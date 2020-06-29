#include "TrianglePrimitive.h"
#include "external\xxhash\xxhash.h"
using namespace cocos2d;

TrianglePrimitive* TrianglePrimitive::create(const cocos2d::Vec2& p1, const cocos2d::Vec2& p2, const Vec2& p3, const cocos2d::Color4F& color)
{
    auto ins = new TrianglePrimitive(p1, p2, p3, color);
    ins->autorelease();
    return ins;
}

TrianglePrimitive::TrianglePrimitive(const cocos2d::Vec2& p1, const cocos2d::Vec2& p2, const Vec2& p3, const cocos2d::Color4F& color)
{
    float intArray[] = {p1.x, p1.y, p2.x,p2.y,p3.x,p3.y,color.r, color.g, color.b,color.a };
    _primitiveHash = XXH32((const void*)intArray, sizeof(intArray), 0);
    
    _vertex_count = 3;
    
    Color4B col = Color4B(color);
    V2F_C4B_T2F a = { Vec2(p1.x, p1.y), col, Tex2F(0.0, 0.0) };
    V2F_C4B_T2F b = { Vec2(p2.x, p2.y), col, Tex2F(0.0,  0.0) };
    V2F_C4B_T2F c = { Vec2(p3.x, p3.y), col, Tex2F(0.0,  0.0) };

    _trianglesbuffer = new V2F_C4B_T2F[6];
    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_trianglesbuffer);
    V2F_C4B_T2F_Triangle triangle = { a, b, c };

    triangles[0] = triangle;
    calculateTriangleDone();
}