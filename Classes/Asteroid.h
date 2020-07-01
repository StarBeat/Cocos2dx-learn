#pragma once
#include "IPrimitive.h"
#include "cocos2d.h"

class Asteroid
{
public:
	Asteroid(float x, float y, int mass, int type);
	~Asteroid();
	IPrimitive* getNode()
	{
		return _body;
	}
private:
	IPrimitive* _body;
	int _mass;
};