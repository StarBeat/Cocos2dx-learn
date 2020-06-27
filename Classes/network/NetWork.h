#pragma once
#include <string>

class NetWork
{
public:
	enum class Type
	{
		Client,
		Server,
		None
	};
	NetWork(Type type, std::string& ip, int port);

private:
	Type _type;
};