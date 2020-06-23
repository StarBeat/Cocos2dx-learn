#include "xnet.h"
#include <cassert>
#include <thread>
#include <iostream>
#include <cassert>
namespace Util
{
	static Queue<Message*> msgPoll;
	bool isInProgress()
	{
#if WIN32
		return WSAEINPROGRESS == WSAGetLastError();
#else
		return EINPROGRESS == errno;
#endif // BX_PLATFORM_WINDOWS
	}

	bool isWouldBlock()
	{
#if WIN32
		return WSAEWOULDBLOCK == WSAGetLastError();
#else
		return EWOULDBLOCK == errno;
#endif // BX_PLATFORM_WINDOWS
	}

	int getLastError()
	{
#if WIN32
		return WSAGetLastError();
#else
		return errno;
#endif // BX_PLATFORM_WINDOWS
	}

	static int connectSocket(SOCKET socket, uint32_t _ip, uint16_t _port)
	{
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = PF_INET;
		addr.sin_addr.s_addr = _ip;
		addr.sin_port = htons(_port);
		return connect(socket, (sockaddr*)&addr, sizeof(sockaddr));
	}

	static bool socketReady(SOCKET socket)
	{
		fd_set rfds;
		FD_ZERO(&rfds);
		fd_set wfds;
		FD_ZERO(&wfds);
		FD_SET(socket, &rfds);
		FD_SET(socket, &wfds);

		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		int result = ::select((int)socket + 1 /*nfds is ignored on windows*/, &rfds, &wfds, NULL, &timeout);
		return result > 0;
	}

	void setNoBlock(SOCKET _socket)
	{
#if WIN32
		unsigned long opt = 1;
		ioctlsocket(_socket, FIONBIO, &opt);
#else
		fcntl(_socket, F_SETFL, O_NONBLOCK);
#endif 
	}

	static void setSockOpts(SOCKET _socket)
	{
		int result;

	/*	int win = 256 << 10;
		result = ::setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char*)&win, sizeof(win));
		result = ::setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char*)&win, sizeof(win));

		int noDelay = 1;
		result = ::setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelay, sizeof(noDelay));*/
	}

	SimpleNet::SimpleNet(int maxConnect):_maxConnect(maxConnect)
	{
#if __WIN
		WSADATA wsaData;
		if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0)
		{
			WSACleanup();
			assert(false);
		}
#endif
	}

	SimpleNet::~SimpleNet()
	{
#if WIN32
		WSACleanup();
#endif
	}

	void SimpleNet::listen(const string& ip, uint32_t port)
	{
		uint32_t _ip = inet_addr(ip.c_str());
		_accept = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		setSockOpts(_accept);
		sockaddr_in addr;
		addr.sin_port = htons(port);
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		int len = sizeof(addr);
		if (SOCKET_ERROR == ::bind(_accept, (sockaddr*)&addr, sizeof(sockaddr)))
		{
			closesocket(_accept);
			std::cout << "端口绑定失败" << std::endl;
			return;
		}
		if( SOCKET_ERROR == ::listen(_accept, SOMAXCONN))
		{
			closesocket(_accept);
			std::cout << "监听失败" << std::endl;
			return;
		}
		std::thread t([this]
			{
				sockaddr_in addr;
				socklen_t len = sizeof(addr);
				while (true)
				{
					if (_shutdown)
					{
						break;
					}
			
					SOCKET socket = ::accept(_accept, (sockaddr*)&addr, &len);
					if (INVALID_SOCKET != socket)
					{
						setNoBlock(socket);
						auto c = new Connection(socket, ntohl(addr.sin_addr.s_addr), ntohs(addr.sin_port), _connections.size());
						_connections.push_back(c->bindNotify([this](Message* msg)
							{
								_recvque.push(msg);
							}));
						std::cout << "accept:"<< socket << std::endl;
					}
				}
			});
		t.detach();
	}

	void SimpleNet::connect(const string& ip, uint32_t port)
	{
		uint32_t _ip = inet_addr(ip.c_str());
		SOCKET _socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		setSockOpts(_socket);
		int ret = connectSocket(_socket, _ip, port);
		if (ret != 0)
		{
			std::cout << "connectSocket err:" << getLastError();
			closesocket(_socket);
		}
		setNoBlock(_socket);
		_connections.push_back((new Connection(_socket, _ip, port, _connections.size()))->bindNotify([this](Message* msg)
			{
				_recvque.push(msg);
			}));
	}

	void SimpleNet::shutdown()
	{
		_shutdown = true;
		for (size_t i = 0; i < _connections.size(); i++)
		{
			delete _connections[i];
		}
		_connections.clear();
		closesocket(_accept);
	}

	Message* SimpleNet::getMessage()
	{
		Message* msg;
		if (!msgPoll.empty())
		{
			msg = msgPoll.pop();
			memset(msg->data, 0, 256);
		}
		else
		{
			msg = new Message();
		}
		return msg;
	}

	void SimpleNet::recycleMessage(Message* msg)
	{
		msgPoll.push(msg);
	}

	void SimpleNet::send(int id, Message* msg)
	{
		if (_connections.size() > id )
		{
			_connections[id]->send(msg);
		}
	}

	void SimpleNet::recv(int id, Message** msg)
	{
		if (_connections.size() > id)
		{
			_connections[id]->recv(msg);
		}
	}

	void SimpleNet::recv(Message** msg)
	{
		if (_recvque.size() > 0)
		{
			*msg = _recvque.pop();
		}
	}

	void Connection::disconnect()
	{
		_id = -1;
		closesocket(_socket);
	}
	void Connection::update()
	{
		static char p[sizeof(uint8_t)];

		while (!_sendque.empty())
		{
			auto msg = _sendque.pop();
			int rt = 0;
			uint8_t len = msg->size + sizeof uint8_t,  offset =0;
			char* p = reinterpret_cast<char*>(&len);
			memcpy(msg->data, p, sizeof uint8_t);
			do
			{
				assert(len != 0&& offset<=255);
				rt = ::send(_socket, (char*)&(msg->data[offset]), len, 0);
				if (rt < 0)
				{
					if (-1 == rt && !isWouldBlock())
					{
						disconnect();
					}
				}
				else
				{
					len -= rt;
					offset += rt;
				}
			} while (len > 0);
			msgPoll.push(msg);
		}

		//if (socketReady(_socket))
		{
			Message* msg = getFreeMessage();
			int rt = ::recv(_socket, (char*)msg->data, 255,0);
			if (rt < 1)
			{
				if (rt == 0)
				{
					std::cout << "对端关闭连接" << std::endl;
					disconnect();
					return;
				}
				else if(!isWouldBlock())
				{
					std::cout << getLastError();
					disconnect();
				}
				msgPoll.push(msg);
				return;
			}
			std::cout << "recv len:" << rt << std::endl;

			uint8_t spliceOffet = 0;
			if (_needSplice)
			{
				msg->size = rt;
				int over = spliceData(msg, 0);
				if (over <= 0)
				{
					msgPoll.push(msg);
					return;
				}
				spliceOffet = rt - over;
				rt = over;
			}

			uint8_t len = 0;
			uint8_t offet = spliceOffet;

			memcpy(p, msg->data + spliceOffet, sizeof(uint8_t));
			len = *reinterpret_cast<uint8_t*>(&p[0]);
			if (len > 256 || len == 0)
			{
				assert(0);
			}
			
			if (len < rt)
			{
				while(len <= rt)//拆包
				{
					Message* sub = getFreeMessage();
					assert(len - sizeof(uint8_t) > 0);
					memcpy(sub->data, msg->data + offet + sizeof(uint8_t), len - sizeof(uint8_t));
					offet += len;
					sub->id = _id;
					sub->size = len - sizeof(uint8_t);
					onData(sub);
					rt -= len;
					if (rt == 0)
					{
						len = 0;
						break;
					}
					memcpy(p, msg->data + offet, sizeof(uint8_t));
					len = *reinterpret_cast<uint8_t*>(&p[0]);
					if (len > 500)
					{
						assert(0);
					}
				}

				if (len > rt)//粘包
				{
					Message* sub = getFreeMessage();
					sub->id = _id;
					sub->size = len - sizeof(uint8_t);
					memcpy(sub->data, msg->data + offet + sizeof(uint8_t), rt - sizeof(uint8_t));
					spliceData(sub, len - rt);
				}
				msgPoll.push(msg);
				return;
			}
			else if (len > rt)
			{
				Message* sub = getFreeMessage();
				sub->id = _id;
				memcpy(sub->data, msg->data + sizeof(uint8_t), rt - sizeof(uint8_t));
				sub->size = len - sizeof(uint8_t);
				int over = spliceData(sub, len - rt);
				msgPoll.push(msg);
				return;
			}
			memmove(msg->data, msg->data + sizeof(uint8_t), rt - sizeof(uint8_t));
			msg->id = _id;
			msg->size = rt - sizeof(uint8_t);
			onData(msg);
		}
	}

	void Connection::send(Message* msg)
	{
		_sendque.push(msg);
	}
	void Connection::recv(Message** msg)
	{
		if (_recvque.size() > 0)
		{
			*msg = _recvque.pop();
		}
	}
	
	Connection* Connection::bindNotify(std::function<void(Message* msg)> notify)
	{
		_notify = notify;
		return this;
	}

	int Connection::spliceData(Message* msg, uint8_t lack)
	{
		_needSplice = true;
		static uint8_t _lack = 0;
		static Message* _msg = nullptr;
		if (_msg != nullptr && _lack != 0)
		{
			if (_lack <= msg->size)
			{
				memcpy(_msg->data, msg->data , _lack);
				onData(_msg);
				int l = msg->size - _lack;
				_msg = nullptr;
				_lack = 0;
				_needSplice = false;
				return l;
			}
			if (_lack > msg->size)
			{
				memcpy(_msg->data, msg->data, msg->size);
				_lack -= msg->size;
				return -1;
			}
		}
		_lack = lack == 0 ? _lack : _lack + lack;
		_msg = msg;
		std::cout << "spliceData rt " << (int)_lack << std::endl;
	}

	void Connection::onData(Message* msg)
	{
		if (_notify.operator bool())
		{
			_notify(msg);
		}
		else
		{
			_recvque.push(msg);
		}
	}
	Message* Connection::getFreeMessage()
	{
		Message* sub;
		if (!msgPoll.empty())
		{
			sub = msgPoll.pop();
			memset(sub->data, 0, 256);
		}
		else
		{
			sub = new Message();
		}
		return sub;
	}
}
