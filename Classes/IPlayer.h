#pragma once
#include "cocos2d.h"
#include "network/NetWork.h"
#include "IPrimitive.h"
#include "GameManager.h"
#include "Rpc.h"

class IPlayer
{
public:
	virtual ~IPlayer() = default;
	IPlayer(IPrimitive* primitive) :_primitive(primitive) 
	{
		_rpc = ::GameManager::Instane()->_network->_rpc;
	}
	void setPosition(const cocos2d::Vec2& pos)
	{
		_primitive->setPosition(pos);
	}
	void setRotation(const cocos2d::Vec2& rot)
	{
		_primitive->setRotationX(rot.x);
		_primitive->setRotationY(rot.y);
	}
	operator IPrimitive*() const
	{
		return _primitive;
	}
	operator IPrimitive*()
	{
		return _primitive;
	}
	virtual void rewpan() = 0;
#pragma region Rpc
	virtual	void move(float x, float y) = 0;

#pragma endregion

protected:
	//virtual bool init();
	Util::Rpc* _rpc;

	bool _isalive = true;
	uint16_t _hp;
	uint16_t _speed;
	uint16_t _rank;
public:
	IPrimitive* _primitive;
	
	friend class PlayerManager;
};