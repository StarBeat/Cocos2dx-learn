#include "NetPlayer.h"
#include <CirclePrimitive.h>
#include "PlayerManager.h"
using namespace cocos2d;

NetPlayer* NetPlayer::create(cocos2d::Vec2 pos, cocos2d::Vec2 rot)
{
	auto ins = new NetPlayer(pos, rot);
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

void NetPlayer::rewpan()
{
}

NetPlayer::~NetPlayer()
{

}

NetPlayer::NetPlayer(cocos2d::Vec2 pos, cocos2d::Vec2 rot) : IPlayer(CirclePrimitive::create({ 0,0 }, 10, 50, 1, 1, Color4F::BLUE))
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
