#pragma once
#include "IPrimitive.h"
#include "cocos2d.h"

class Asteroid
{
public:
	Asteroid();
	~Asteroid();

private:
	IPrimitive* _body;
};