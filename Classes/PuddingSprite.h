#pragma once
#include <cocos2d.h>
#include <PuddingEffect.h>

class PuddingSprite : public cocos2d::Node
{
public:
	static PuddingSprite* create(const std::string& tex) {
		auto ins = new PuddingSprite(tex);
		ins->autorelease();
		return ins;
	}
	PuddingSprite(const std::string& tex);
	~PuddingSprite();

	void initParticlePosition(float x, float y);
	void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
private:
	cocos2d::BlendFunc _blendFunc;
	cocos2d::Texture2D* _texture;
	cocos2d::Mat4 _ratioTransform;
	PuddingEffect* _pdeff;
};