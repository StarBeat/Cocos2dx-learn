#include "RectPrimitive.h"
#include "external\xxhash\xxhash.h"

using namespace cocos2d;

RectPrimitive::RectPrimitive(const cocos2d::Vec2& origin, const cocos2d::Vec2& destination, const cocos2d::Color4F& color, float borderWidth, const cocos2d::Color4F& borderColor)
{
    //todo È¥µôÖØ¸´¼ÆËã
    float intArray[] = { origin.x, origin.y, destination.x,destination.y,borderWidth, color.r, color.g, color.b,color.a , borderColor .r, borderColor.g, borderColor .b, borderColor .a};
    _primitiveHash = XXH32((const void*)intArray, sizeof(intArray), 0);
    Vec2 vertices[] = {
        origin,
        Vec2(destination.x, origin.y),
        destination,
        Vec2(origin.x, destination.y)
    };

   createPolygonTriangle(vertices, 4, color, borderWidth, borderColor);
   calculateTriangleDone();
}

RectPrimitive* RectPrimitive::create(const cocos2d::Vec2& origin, const cocos2d::Vec2& destination, const cocos2d::Color4F& color, float borderWidth, const cocos2d::Color4F& borderColor)
{
    auto ins = new RectPrimitive(origin, destination, color, borderWidth, borderColor);
    ins->autorelease();
    return ins;
}
