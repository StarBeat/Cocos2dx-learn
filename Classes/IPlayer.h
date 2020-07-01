#pragma once
#include "cocos2d.h"
#include "network/NetWork.h"
#include "IPrimitive.h"
#include "GameManager.h"
#include "Rpc.h"
#include "PhysicExt.h"
#include "Light2d.h"

using cocos2d::PhysicsBody;

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
	virtual bool init()
	{
		auto pb = PhysicsBody::createCircle(10);
		pb->setContactTestBitmask(::PhysicEx::NODE_BIT::ASTEROID_BIT | ::PhysicEx::NODE_BIT::LPLAYER_BIT | ::PhysicEx::NODE_BIT::NPLAYER_BIT);
		pb->setGroup(::PhysicEx::NODE_GROUP::PLAYER_GROUP);
		pb->setTag(::PhysicEx::NODE_TAG::PLAYER_TAG);
		_primitive->addComponent(pb);
		Light2d* lt = Light2d::create();
		lt->setLightSize(300, 150);
		_primitive->addChild(lt);
		return true;
	}
	Util::Rpc* _rpc;

	bool _isalive = true;
	uint16_t _hp;
	uint16_t _speed;
	uint16_t _rank;
public:
	IPrimitive* _primitive;
	
	friend class PlayerManager;
};