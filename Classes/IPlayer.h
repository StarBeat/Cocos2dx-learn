#pragma once
#include "cocos2d.h"
#include "network/NetWork.h"
#include "IPrimitive.h"
#include "GameManager.h"
#include "Rpc.h"
#include "PhysicExt.h"
#include "GameScene.h"
#include <Asteroid.h>

using cocos2d::PhysicsBody;
using cocos2d::Label;
using cocos2d::EventCustom;
using cocos2d::MoveBy;

class IPlayer
{
public:
	virtual ~IPlayer() = default;
	IPlayer(IPrimitive* primitive, std::string name) :_primitive(primitive), _name(name), _hp(100), _rank(1), _mass(50)
	{
		_primitive->_owner = this;
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

	void evNotifyAsManager()
	{
		auto ev = new EventCustom(EV_EAT);
		ev->retain();
		ev->setUserData(_primitive);
		GameScene::gameScene->getEventDispatcher()->dispatchEvent(ev);
	}
	virtual void death()
	{
		_isalive = false;
		_primitive->retain();
		_primitive->setActive(false);
		//_primitive->removeFromParent();
	 	_primitive->stopAllActions();
		_primitive->setVisible(false);
		_primitive->getPhysicsBody()->setEnabled(false);
	}
	virtual void respwan()
	{
		_isalive = true;
		_hp = 100;
		_rank = 1;
		_mass = 50;
		_primitive->setActive(true);
		_primitive->setVisible(true);
		_primitive->getPhysicsBody()->setEnabled(true);
		updateData();
	}
	void takeDamage(int da)
	{
		_hp -= da;
		if (_hp <= 0)
		{
			death();
		}
	}

	void onContact(IPlayer* other)
	{
		if (_rank == other->_rank)
		{
			_primitive->runAction(MoveBy::create(1.0, other->_primitive->getPosition())->reverse());
			other->_primitive->runAction(MoveBy::create(1.0, _primitive->getPosition())->reverse());
			int damage = _mass / 5;
			takeDamage(damage);
			other->takeDamage(damage);
		}
		else if (_rank > other->_rank)
		{
			_mass += other->_mass;
			other->death();
		}
		else
		{
			other->_mass += _mass;
			death();
		}
		updateData();
		other->updateData();
	}

	void onContact(Asteroid* other)
	{
		if (_mass >= other->getMass())
		{
			_mass += other->getMass();
			other->death();
			evNotifyAsManager();
		}
		else
		{
			int damage = other->getMass() / 5;
			other->runAction(MoveBy::create(1.0, _primitive->getPosition())->reverse());
			_mass -= damage;
			takeDamage(damage);
		}
		updateData();
	}

	void updateData()
	{
		if (_isalive)
		{
			if (_mass >= (_rank + 1) * 100)
			{
				++_rank;
			}
			else
			{
				if (_rank > 1)
				{
					--_rank;
				}
			}
			float scale = 1 + _rank / 5;
			_primitive->setScale(scale);
		}
	}


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
		auto label = Label::createWithTTF(_name, "fonts/Marker Felt.ttf", 16);
		label->setTextColor(cocos2d::Color4B::GRAY);
		label->setPositionY(20);
		_primitive->addChild(label);
		return true;
	}
	
	Util::Rpc* _rpc;
	std::string _name;

	bool _isalive = true;
	uint16_t _hp;
	uint16_t _mass;
	uint16_t _speed;
	uint16_t _rank;
public:
	IPrimitive* _primitive;
	
	friend class PlayerManager;
};