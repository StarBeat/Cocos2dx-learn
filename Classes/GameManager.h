#pragma once
#include "ISingle.h"
#include "cocos2d.h"

class B2Physic;
class NetWork;
class b2World;
extern const char* EV_EAT;

class GameManager : public ISingle<GameManager>
{
public:

	bool moduleInit(cocos2d::PhysicsWorld* pw);
	bool delayInit(cocos2d::Node* n);
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
	NetWork* _network = nullptr;
	int _seed;
	B2Physic* _b2physic = nullptr;
	b2World* _b2world = nullptr;
private:
	void physicModuleInit(cocos2d::PhysicsWorld* pw);
};

