#include "PuddingSprite.h"
#include "b2Physic.h"
USING_NS_CC;

PuddingSprite::PuddingSprite(const std::string& tex) : _ratioTransform(Mat4::IDENTITY)
{
    _texture = Director::getInstance()->getTextureCache()->addImage(tex);
    _pdeff = new PuddingEffect(_texture);
    _ratioTransform.scale(B2Physic::PTM_RATIO, B2Physic::PTM_RATIO, 1);
}

PuddingSprite::~PuddingSprite()
{
}

void PuddingSprite::initParticlePosition(float x, float y)
{
    _pdeff->initData(PuddingEffect::Box, { x,y });
}

void PuddingSprite::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
    Mat4 newMV;
    Mat4::multiply(_modelViewTransform, _ratioTransform, &newMV);
    _pdeff->draw(renderer, newMV, flags);
}
