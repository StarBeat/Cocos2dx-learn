#include "LocalPlayer.h"
#include "CirclePrimitive.h"
#include "PlayerManager.h"

using namespace cocos2d;

LocalPlayer::LocalPlayer(const cocos2d::Vec2& pos, const cocos2d::Vec2& rot) :IPlayer(CirclePrimitive::create({ 0,0 }, 10, 50, 1, 1, Color4F::BLUE))
{
	_selfid = PlayerManager::Instane()->_selfid;
	_primitive->setPosition(pos);
	_primitive->setRotation(rot.x);
}
LocalPlayer* LocalPlayer::create(const Vec2& pos, const Vec2& rot)
{
	auto ins = new LocalPlayer(pos, rot);
	if (ins && ins->init())
	{
		return ins;
	}
	return nullptr;
}

void LocalPlayer::move(float x, float y)
{
	if (_isalive)
	{
		_primitive->setPosition(_primitive->getPosition() + Vec2{x, y});
		if (_rpc->isServer())
		{
			_rpc->call("move", x, y);
		}
		else
		{
			_rpc->call(_selfid, "move", x, y);
		}
	}
}

void LocalPlayer::rewpan()
{
}

LocalPlayer::~LocalPlayer()
{
}


bool LocalPlayer::init()
{
	IPlayer::init();
	_primitive->getPhysicsBody()->setCategoryBitmask(::PhysicEx::NODE_BIT::LPLAYER_BIT);
	return true;
}
