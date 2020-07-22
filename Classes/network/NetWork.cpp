#include "NetWork.h"
#include "PlayerManager.h"

NetWork::NetWork(std::string& ip, int port):_ip(ip),_port(port)
{
	auto rhis = PlayerManager::Instane();
	_rpc = new Rpc(std::bind(&PlayerManager::bindFuncs, rhis, std::placeholders::_1));
}

void NetWork::asServer()
{
	_type = Type::Server;
	_rpc->asServer(_port);
}

void NetWork::asClient()
{
	_type = Type::Client;
	_rpc->asClient(_ip, _port);
}

void NetWork::disConnect(int id)
{
	_rpc->disconnect(id);
}

void NetWork::update(float dt)
{
	_rpc->update();
}
