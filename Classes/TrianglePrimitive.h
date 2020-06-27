#pragma once
#include "IPrimitive.h"

class TrianglePrimitive : public IPrimitive
{
public:
	static TrianglePrimitive* create(const cocos2d::Vec2& p1, const cocos2d::Vec2& p2, const Vec2& p3, const cocos2d::Color4F& color);
	
	~TrianglePrimitive() {}

private:
	TrianglePrimitive(const cocos2d::Vec2& p1, const cocos2d::Vec2& p2, const Vec2& p3, const cocos2d::Color4F& color);
};

