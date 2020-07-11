#pragma once
#include "IPrimitive.h"
#include "cocos2d.h"

class Asteroid
{
public:
	Asteroid(float x, float y, int mass, float density, int type, float r, float g, float b);
	~Asteroid();
	IPrimitive* getNode()
	{
		return _body;
	}
	int getMass()
	{
		return _mass;
	}
	void runAction(cocos2d::Action* action)
	{
		_body->runAction(action);
	}
	void death();
private:
	IPrimitive* _body;
	int _mass;
};