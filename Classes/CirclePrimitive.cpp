#include "CirclePrimitive.h"
#include "external\xxhash\xxhash.h"

using namespace cocos2d;

CirclePrimitive* CirclePrimitive::create(const Vec2& center, float radius, unsigned int segments, float scaleX, float scaleY, const Color4F& color, float borderWidth, const cocos2d::Color4F& borderColor)
{
    auto ins = new CirclePrimitive(center, radius, segments, scaleX, scaleY, color, borderWidth, borderColor);
    ins->autorelease();
    return ins;
}

CirclePrimitive::CirclePrimitive(const Vec2& center, float radius, unsigned int segments, float scaleX, float scaleY, const Color4F& color, float borderWidth, const cocos2d::Color4F& borderColor)
{
    float intArray[] = { center.x, center.y, radius, segments,  color.r, color.g, color.b,color.a , scaleX, scaleY, borderWidth, borderColor.r,borderColor.g, borderColor.b, borderColor.a};
    _primitiveHash = XXH32((const void*)intArray, sizeof(intArray), 0);
    const float coef = 2.0f * (float)M_PI / segments;
    Vec2* vertices = new (std::nothrow) Vec2[segments];
    for (unsigned int i = 0; i < segments; i++)
    {
        float rads = i * coef;
        GLfloat j = radius * cosf(rads) * scaleX + center.x;
        GLfloat k = radius * sinf(rads) * scaleY + center.y;

        vertices[i].x = j;
        vertices[i].y = k;
    }
    createPolygonTriangle(vertices, segments, color, borderWidth, borderColor);
    CC_SAFE_DELETE_ARRAY(vertices);
    calculateTriangleDone();
}