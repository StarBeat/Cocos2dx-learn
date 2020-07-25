#include "Asteroid.h"
#include "DotPrimitive.h"
#include "RectPrimitive.h"
#include "CirclePrimitive.h"
#include "PhysicExt.h"

using namespace cocos2d;

Asteroid::Asteroid(float x, float y, int mass, float density, int type, float r, float g, float b)
{
	float w = mass / density;
	float h = mass / density;
	auto color = Color4F(r,g,b,1);
	printf("(%f,%f)(%f, %f) %d\n",x,y, w, h, type);
	switch (type)
	{
	case 0:
	{

		_body = CirclePrimitive::create(Vec2::ZERO, w, 10, 1, 1, color);
		_body->setPosition(Vec2(x, y));
		auto pb = PhysicsBody::createCircle(w);
		_body->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
		pb->setTag(::PhysicEx::NODE_TAG::ASTEROID_TAG);
		pb->setContactTestBitmask(::PhysicEx::NODE_BIT::LPLAYER_BIT | ::PhysicEx::NODE_BIT::NPLAYER_BIT);
		pb->setCategoryBitmask(::PhysicEx::NODE_BIT::ASTEROID_BIT);
		pb->setCollisionBitmask(0);
		_body->addComponent(pb);
		break;
	}
	case 1:
	{
		//_body = RectPrimitive::create(Vec2(x, y), Vec2(x + 2, y + h), color);
		_body = DotPrimitive::create(Vec2::ZERO, w, color);
		_body->setPosition(Vec2(x, y));
		auto pb = PhysicsBody::createBox({ 2*w,2 * w });
		_body->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
		pb->setTag(::PhysicEx::NODE_TAG::ASTEROID_TAG);
		pb->setContactTestBitmask(::PhysicEx::NODE_BIT::LPLAYER_BIT | ::PhysicEx::NODE_BIT::NPLAYER_BIT);
		pb->setCollisionBitmask(0);
		pb->setCategoryBitmask(::PhysicEx::NODE_BIT::ASTEROID_BIT);
		_body->addComponent(pb);
		break;
	}
	case 2:
	{
		_body = DotPrimitive::create(Vec2::ZERO, w, color);
		_body->setPosition(Vec2(x, y));
		auto pb = PhysicsBody::createBox({ 2 * w,2 * w });
		_body->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
		pb->setTag(::PhysicEx::NODE_TAG::ASTEROID_TAG);
		pb->setContactTestBitmask(::PhysicEx::NODE_BIT::LPLAYER_BIT | ::PhysicEx::NODE_BIT::NPLAYER_BIT);
		pb->setCategoryBitmask(::PhysicEx::NODE_BIT::ASTEROID_BIT);
		pb->setCollisionBitmask(0);
		_body->addComponent(pb);
		break;
	}
	default:
		break;
	}
	_body->_owner = this;
}

Asteroid::~Asteroid()
{
}

void Asteroid::death()
{
	_body->removeFromParent();
}
