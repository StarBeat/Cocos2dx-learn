#include "RectPrimitive.h"
using namespace cocos2d;

RectPrimitive::RectPrimitive(const cocos2d::Vec2& origin, const cocos2d::Vec2& destination, const cocos2d::Color4F& color, float borderWidth, const cocos2d::Color4F& borderColor)
{
    Vec2 vertices[] = {
        origin,
        Vec2(destination.x, origin.y),
        destination,
        Vec2(origin.x, destination.y)
    };

   createPolygonTriangle(vertices, 4, color, borderWidth, borderColor);
   _effect->genBindBuffer(_trianglesbuffer, _vertex_count);
}

RectPrimitive* RectPrimitive::create(const cocos2d::Vec2& origin, const cocos2d::Vec2& destination, const cocos2d::Color4F& color, float borderWidth, const cocos2d::Color4F& borderColor)
{
    auto ins = new RectPrimitive(origin, destination, color, borderWidth, borderColor);
    ins->autorelease();
    return ins;
}

void RectPrimitive::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    _effect->use(transform);
    _effect->draw(_vertex_count);
}
