#pragma once
#include "IPrimitive.h"
#include "cocos2d.h"

class DotPrimitive : public IPrimitive
{
public:
	static DotPrimitive* create(const cocos2d::Vec2& pos, float radius, const cocos2d::Color4F& color);
	~DotPrimitive();
private:
	DotPrimitive(const cocos2d::Vec2& pos, float radius, const cocos2d::Color4F& color);
};

