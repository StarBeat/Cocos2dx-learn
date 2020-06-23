#pragma once

#if WIN32
#include <ws2tcpip.h>
#define socklen_t int32_t
#else
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h> // inet_addr
#include <netinet/in.h>
typedef int SOCKET;
typedef linger LINGER;
typedef hostent HOSTENT;
typedef in_addr IN_ADDR;
#define closesocket close
#endif

#include <string>
#include "Queue.hpp"
#include <functional>
#include <chrono>  
#include <thread> 
using std::string;
using std::vector;
namespace Util 
{


struct Message
{
	uint16_t size;
	int32_t id;
	char data[256];
	~Message() { size = 0;}
public:
	///转发
	void rawwrite(const char* s, size_t size)
	{
		memcpy(data, s, size);
	}
	//外部写入数据，保留分包长度
	void write(const char* s, size_t size) 
	{
		memcpy(data + sizeof(uint8_t), s, size);
	}
private:
	friend class SimpleNet;
	friend class Connection;
	Message(uint16_t s = 0, uint16_t i = -1):size(s), id(i) { }
};

struct Connection
{
public:
	Connection(SOCKET s, uint32_t ip, uint32_t port, uint16_t id): _socket(s), _ip(ip), _port(port),_id(id) {}
	~Connection() { disconnect(); }
	void disconnect();
	void update();
	void send(Message* msg);
	void recv(Message**);
	Connection* bindNotify(std::function<void(Message* msg)> notify);
private:
	int spliceData(Message* msg, uint8_t lack);
	void onData(Message*);
	Message* getFreeMessage();
private:
	int _id;
	SOCKET _socket;
	uint32_t _ip;
	uint32_t _port;
	Queue<Message*> _sendque;
	Queue<Message*> _recvque;
	std::function<void(Message* msg)> _notify;
	bool _needSplice = false;
	
	friend class SimpleNet;
};

class SimpleNet
{
public:
	SimpleNet(int maxConnect = 1);
	~SimpleNet();
	void listen(const string& ip = "127.0.0.1", uint32_t port = 666);
	void connect(const string& ip = "127.0.0.1", uint32_t port = 666);
	void shutdown();
	Message* getMessage();
	void recycleMessage(Message*);
	void send(int id, Message* msg);
	void recv(int id, Message** msg);
	void recv(Message** msg);
	void run()
	{
		std::thread t(
			[this]
			{
				while (true)
				{
					update();
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
				}
			});
		t.detach();
	}
private:
	void update()
	{
		if (!_shutdown)
		{
			for (size_t i = 0; i < _connections.size(); i++)
			{
				if (_connections.size() == 0)
				{
					break;
				}
				if (-1 == _connections[i]->_id)
				{
					_connections.erase(_connections.begin() + i);
					continue;
				}
				_connections[i]->update();
			}
		}
	}
	vector<Connection*> _connections;
	Queue<Message*> _recvque;
	SOCKET _accept;
	uint32_t _maxConnect;
	bool _shutdown = false;
};
}