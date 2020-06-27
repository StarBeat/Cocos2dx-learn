#include "Rpc.h"
#include <sstream>
#include <thread>
#include "xnet.h"

namespace Util
{
using std::ostringstream;
void Rpc::connect(int id)
{
	if (_type == CSType::RPC_Server)
	{
		call("connect", id);
		for (auto i : _ids)
		{
			single_call(i, "connect", i);
		}
		single_call(id, "connect", SERVERID);
	}
	_bindEnty(id);
	_ids.push_back(id);
}
void Rpc::disconnect(int id)
{
	if (id == SERVERID)
	{
		//shutdown
		shutdown();
	}
	//delete bind
	unbind(id);
	if (_type == CSType::RPC_Server)
	{
		call("disconnect", id);
	}
}
Rpc::Rpc(std::function<void(int)> bindEnty) : _bindEnty(bindEnty)
{
	this->bind(DEFAULT_BINDID, "connect", &Rpc::connect, this);
	this->bind(DEFAULT_BINDID, "disconnect", &Rpc::connect, this);
}

Rpc::~Rpc()
{
	if (_type == CSType::RPC_Client)
	{
		call("disconnect", SERVERID);
	}
	_net->shutdown();
	delete _net;
}

void Rpc::asClient(std::string& ip, int port) 
{
	_type = CSType::RPC_Client;
	_net = new SimpleNet(1);
	_net->connect(ip, port);
	_net->run();
	call("connect", -1);
}

void Rpc::asServer(int port)
{
	_type = CSType::RPC_Server;
	_net = new SimpleNet(64);
	_net->listen("", port);
	_net->run();
}

void Rpc::shutdown()
{
	//todo
}

void Rpc::send(Message* msg)
{
	_net->send(msg->id, msg);
}

void Rpc::recv(Message** msg)
{
	_net->recv(msg);
}

void Rpc::serverRun()
{
	if (_type != CSType::RPC_Server)
	{
		return;
	}
	std::thread t([this] 
		{
			while (true)
			{
				//Message* msg = nullptr;
				//recv(&msg);
				//ByteStream bs((char*)msg->data, msg->size);
				//Serializer ds(bs);
				//_net->recycleMessage(msg);
				//std::string funcname;
				//ds >> funcname;
				//if (funcname.compare("disconnect") == 0 || funcname.compare("connect") == 0)
				//{
				//	int cid;
				//	ds >> cid;
				//	if (cid == -1)
				//	{
				//		ds<<msg->id;
				//	}
				//	else
				//	{
				//		ds << cid;
				//	}
				//}
				//call_(funcname, ds.current(), ds.size());
			}
		});
	t.detach();
}

void Rpc::update()
{
	Message* msg = nullptr;
	recv(&msg);
	if (msg == nullptr)
		return;
	ByteStream bs((char*)msg->data, msg->size);
	Serializer ds(bs);
	int sid;
	std::string funcname;
	ds >> funcname;
	ds >> sid;
	if (funcname.compare("disconnect") == 0 || funcname.compare("connect") == 0)
	{
		int cid;
		ds >> cid;
		if (cid == -1)
		{
			ds << msg->id;
		}
		else
		{
			ds << cid;
		}
		call_(DEFAULT_BINDID, funcname, ds.current(), ds.lavesize());
		_net->recycleMessage(msg);
		return;
	}
	if (_type == CSType::RPC_Server)
	{
		for (auto id : _ids)
		{
			if (msg->id != id)
			{
				Message* transitMsg = _net->getMessage();
				transitMsg->id = id;
				transitMsg->size = msg->size;
				msg->rawwrite(ds.data(), ds.size());
				send(msg);
			}
		}
	}
	call_(sid, funcname, ds.current(), ds.lavesize());
}

void Rpc::unbind(int id)
{
	if (_handlers.find(id) != _handlers.end())
	{
		_handlers.erase(id);
		_ids.remove(id);
	}
}

void Rpc::call_(int id, std::string name, const char* data, int len)
{
	if (_handlers.find(id) == _handlers.end() && _handlers[id].find(name) == _handlers[id].end())
	{
		return;
	}
	auto fun = _handlers[id][name];
	if (fun.operator bool())
	{
		fun(data, len);
	}
}
void Rpc::net_call(Serializer& ds, int id)
{
	Message* msg = _net->getMessage();
	msg->id = id;
	msg->size = ds.size();
	msg->write(ds.data(), ds.size());
	send(msg);
	ds.clear();
}
}