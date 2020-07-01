#include "Asteroid.h"
#include "DotPrimitive.h"
#include "RectPrimitive.h"
#include "CirclePrimitive.h"
using namespace cocos2d;

Asteroid::Asteroid(float x, float y, int mass, int type)
{
	float w = mass;
	float h = mass;
	auto color = Color4F::RED;
	switch (type)
	{
	case 0:
		_body = CirclePrimitive::create(Vec2(x, y), w, 50, 1, 1, color);
		break;
	case 1:
		_body = RectPrimitive::create(Vec2(x, y), { w,h }, color);
		break;
	case 2:
		_body = DotPrimitive::create(Vec2(x, y), 50, color);
		break;
	default:
		break;
	}
}

Asteroid::~Asteroid()
{
}