#include "GameManager.h"
#include "PhysicExt.h"
#include "LightingManager.h"
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
	LightingManager::Instane()->init();
	return true;
}
