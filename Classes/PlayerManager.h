#pragma once
#include "NetPlayer.h"
#include "LocalPlayer.h"
#include "ISingle.h"

class PlayerManager : public ISingle<PlayerManager>
{
public:
	PlayerManager() = default;
	IPlayer* createNetPlayer(cocos2d::Vec2 pos = 0, cocos2d::Vec2 rot = 0);
	IPlayer* createLocalPlayer(cocos2d::Vec2 pos = 0, cocos2d::Vec2 rot = 0);
	IPlayer* respwanNetPlayer(cocos2d::Vec2 pos = 0, cocos2d::Vec2 rot = 0);
	IPlayer* respwanLocalPlayer(cocos2d::Vec2 pos = 0, cocos2d::Vec2 rot = 0);
};

