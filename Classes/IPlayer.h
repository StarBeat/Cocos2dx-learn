#pragma once
#include "cocos2d.h"
#include "network/NetWork.h"
#include "IPrimitive.h"

class IPlayer
{
public:
	virtual ~IPlayer() = default;
	IPlayer(IPrimitive* primitive) :_primitive(primitive) {}
protected:
//	virtual bool init() = 0;
public:
	friend class PlayerManager;
	
	IPrimitive* _primitive;
	NetWork* _network; 
};