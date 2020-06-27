#pragma once
#include "ISingle.h"
#include "cocos2d.h"

class GameManager : public ISingle<GameManager>
{
public:
	bool moduleInit(cocos2d::PhysicsWorld* pw);
	bool delayInit();
	~GameManager() = default;
	GameManager() = default;
};

