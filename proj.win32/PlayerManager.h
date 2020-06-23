#pragma once
#include "NetPlayer.h"
#include "LocalPlayer.h"

class PlayerManager 
{
private:
	PlayerManager() = default;
	static PlayerManager instance;
public:
	static PlayerManager& getInstance(){ return instance; }
	IPlayer* createNetPlayer();
	IPlayer* createLocalPlayer();
};

