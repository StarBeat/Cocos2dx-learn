#pragma once
#include <string>
#include "include\Rpc.h"
using namespace Util;

class NetWork
{
public:
	enum class Type
	{
		Client,
		Server,
		None
	};
	NetWork(std::string& ip, int port);
	~NetWork()
	{
		delete _rpc;
	}
	void asServer();
	void asClient();
	bool isServer()
	{
		return _rpc->isServer();
	}
	void disConnect(int selfid);
	void update(float dt);
public:
	Rpc* _rpc;
	Type _type;
private:
	std::string _ip;
	int _port;
};