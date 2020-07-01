#include "GameManager.h"
#include "PhysicExt.h"
#include "LightingManager.h"
#include "InstanceRenderCommand.h"
#include "PlayerManager.h"
#include "Random.h"
#include "PlayerManager.h"

#include <chrono>
using namespace cocos2d;

template<>
GameManager* ISingle<GameManager>::instance = new GameManager();
PhysicsWorld* ::PhysicEx::instance = nullptr;

void physicModuleInit(PhysicsWorld* pw)
{
	pw->setGravity(cocos2d::Vec2::ZERO);
	::PhysicEx::init(pw);
}
bool GameManager::moduleInit(cocos2d::PhysicsWorld* pw)
{
	physicModuleInit(pw);
	return true;
}

bool GameManager::delayInit()
{
	PlayerManager::Instane()->init();
	LightingManager::Instane()->init();
	InstanceRenderCommand::init();
	return true;
}

GameManager::GameManager()
{
	_network = new NetWork(std::string("127.0.0.1"), 12345);
}

void GameManager::asServer()
{
	auto now = std::chrono::system_clock::now().time_since_epoch();
	_seed = std::chrono::duration_cast<std::chrono::microseconds>(now).count();
	_network->asServer(_seed);
	Random::Instane()->init(_seed);
}

void GameManager::asClient()
{
	_network->asClient();
}

void GameManager::setSeed(int seed)
{
	if (_network->_type == NetWork::Type::Client)
	{
		Random::Instane()->init(seed);
		_seed = seed;
	}
}
