#include "GameManager.h"
#include "PhysicExt.h"
#include "LightingManager.h"
#include "InstanceRenderCommand.h"
#include "PlayerManager.h"
#include "Random.h"
#include "PlayerManager.h"
#include "AsteroidManager.h"
#include "b2Physic.h"
#include "network\NetWork.h"

#include <chrono>
using namespace cocos2d;

const char* EV_EAT = "Ev_eat";
template<>
GameManager* ISingle<GameManager>::instance = new GameManager();
PhysicsWorld* ::PhysicEx::instance = nullptr;

void GameManager::physicModuleInit(PhysicsWorld* pw)
{
	pw->setGravity(cocos2d::Vec2::ZERO);
	_b2physic = B2Physic::create();
	_b2physic->retain();
	_b2world = _b2physic->_world;
	::PhysicEx::init(pw);
}
bool GameManager::moduleInit(cocos2d::PhysicsWorld* pw)
{
	physicModuleInit(pw);
	return true;
}

bool GameManager::delayInit(cocos2d::Node* n)
{
	PlayerManager::Instane()->init();
	LightingManager::Instane()->init();
	InstanceRenderCommand::init();
	n->addChild(_b2physic);
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
	_network->asServer();
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
		AsteroidManager::Instane()->init();
		_seed = seed;
	}
}
