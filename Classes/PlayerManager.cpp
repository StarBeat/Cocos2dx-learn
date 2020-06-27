#include "PlayerManager.h"

template<>
PlayerManager* ISingle<PlayerManager>::instance = new PlayerManager();

IPlayer* PlayerManager::createNetPlayer(cocos2d::Vec2 pos, cocos2d::Vec2 rot)
{
	return nullptr;
}

IPlayer* PlayerManager::createLocalPlayer(cocos2d::Vec2 pos, cocos2d::Vec2 rot)
{
	return nullptr;
}

IPlayer* PlayerManager::respwanNetPlayer(cocos2d::Vec2 pos, cocos2d::Vec2 rot)
{
	return nullptr;
}

IPlayer* PlayerManager::respwanLocalPlayer(cocos2d::Vec2 pos, cocos2d::Vec2 rot)
{
	return nullptr;
}
