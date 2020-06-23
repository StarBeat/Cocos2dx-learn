#pragma once
#include "cocos2d.h"
#include "NetWork.h"

class IPlayer
{
public:
	virtual ~IPlayer() = default;
	
protected:
	virtual bool init() = 0;
protected:
	friend class PlayerManager;
	
	cocos2d::Vec2 _position;
	cocos2d::Vec2 _rotation;
	NetWork* _network;
};