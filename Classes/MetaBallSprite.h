#pragma once
#include <cocos2d.h>

class MetaBallSprite : public cocos2d::Sprite
{
public:
	static MetaBallSprite* create(const std::string& path);
	MetaBallSprite(const std::string& path);
	~MetaBallSprite();
	virtual void draw(cocos2d::Renderer* renderer, const  cocos2d::Mat4& transform, uint32_t flags) override;
	virtual bool init();
};