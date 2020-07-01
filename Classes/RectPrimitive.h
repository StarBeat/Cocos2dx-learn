#pragma once
#include "cocos2d.h"
#include "IPrimitive.h"

class RectPrimitive : public IPrimitive
{
public:
	static RectPrimitive* create(const cocos2d::Vec2& origin, const cocos2d::Vec2& destination, const cocos2d::Color4F& color, float borderWidth = 0, const cocos2d::Color4F& borderColor = {0,0,0,0});
	~RectPrimitive() { }
private:
	RectPrimitive(const cocos2d::Vec2& origin, const cocos2d::Vec2& destination, const cocos2d::Color4F& color, float borderWidth, const cocos2d::Color4F& borderColor);
};

