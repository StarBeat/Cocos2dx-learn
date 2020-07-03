#pragma once
#include "ISingle.h"
#include "cocos2d.h"
#include "network\NetWork.h"

extern const char* EV_EAT;

class GameManager : public ISingle<GameManager>
{
public:

	bool moduleInit(cocos2d::PhysicsWorld* pw);
	bool delayInit();
	~GameManager() 
	{
		delete _network;
	}
	GameManager();

	void asServer();
	void asClient();
#pragma region rpc
	void setSeed(int seed);
#pragma endregion
	NetWork* _network;
	int _seed;
};

