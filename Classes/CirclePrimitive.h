#pragma once
#include "IPrimitive.h"
#include "cocos2d.h"

class CirclePrimitive : public IPrimitive
{
public:
	CirclePrimitive* create(const cocos2d::Vec2& center, float radius, unsigned int segments, float scaleX, float scaleY, const cocos2d::Color4F& color, float borderWidth, const cocos2d::Color4F& borderColor);
	~CirclePrimitive() {  }
private:
	CirclePrimitive(const cocos2d::Vec2& center, float radius, unsigned int segments, float scaleX, float scaleY, const cocos2d::Color4F& color, float borderWidth, const cocos2d::Color4F& borderColor);
	
};

