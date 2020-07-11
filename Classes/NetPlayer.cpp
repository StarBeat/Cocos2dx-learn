#include "NetPlayer.h"
#include <CirclePrimitive.h>
#include "PlayerManager.h"
using namespace cocos2d;

NetPlayer* NetPlayer::create(cocos2d::Vec2 pos, cocos2d::Vec2 rot, std::string name)
{
	auto ins = new NetPlayer(pos, rot, name);
	if (ins && ins->init())
	{
		return ins;
	}
	return nullptr;
}

void NetPlayer::move(float x, float y)
{
	if (_isalive)
	{
		_primitive->setPosition(_primitive->getPosition() + Vec2{ x, y });
	}
}

void NetPlayer::death()
{
	IPlayer::death();
}

void NetPlayer::respwan()
{
	IPlayer::respwan();
}

void NetPlayer::respwanNetPlayer(const Vec2Ex& pos, const Vec2Ex& rot)
{
	setPosition(pos);
	setRotation(rot);
	respwan();
	_primitive->setPosition(pos);
	_primitive->setRotation(rot.x);
}

NetPlayer::~NetPlayer()
{

}

NetPlayer::NetPlayer(cocos2d::Vec2 pos, cocos2d::Vec2 rot, std::string name) : IPlayer(CirclePrimitive::create({ 0,0 }, 10, 50, 1, 1, Color4F::BLUE), name)
{
	_primitive->setPosition(pos);
	_primitive->setRotation(rot.x);
}

bool NetPlayer::init()
{
	IPlayer::init();
	_primitive->getPhysicsBody()->setCategoryBitmask(::PhysicEx::NODE_BIT::NPLAYER_BIT);

	return true;
}
