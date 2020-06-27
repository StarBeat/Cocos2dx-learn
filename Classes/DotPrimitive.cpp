#include "DotPrimitive.h"
using namespace cocos2d;

DotPrimitive::DotPrimitive(const Vec2& pos, float radius, const Color4F& color)
{
	_vertex_count = 6;
    V2F_C4B_T2F a = { Vec2(pos.x - radius, pos.y - radius), Color4B(color), Tex2F(-1.0, -1.0) };
    V2F_C4B_T2F b = { Vec2(pos.x - radius, pos.y + radius), Color4B(color), Tex2F(-1.0,  1.0) };
    V2F_C4B_T2F c = { Vec2(pos.x + radius, pos.y + radius), Color4B(color), Tex2F(1.0,  1.0) };
    V2F_C4B_T2F d = { Vec2(pos.x + radius, pos.y - radius), Color4B(color), Tex2F(1.0, -1.0) };

    _trianglesbuffer = new V2F_C4B_T2F[6];
    V2F_C4B_T2F_Triangle* triangles = (V2F_C4B_T2F_Triangle*)(_trianglesbuffer);
    V2F_C4B_T2F_Triangle triangle0 = { a, b, c };
    V2F_C4B_T2F_Triangle triangle1 = { a, c, d };
    triangles[0] = triangle0;
    triangles[1] = triangle1;
    _effect->genBindBuffer(_trianglesbuffer, _vertex_count);
}

DotPrimitive* DotPrimitive::create(const cocos2d::Vec2& pos, float radius, const cocos2d::Color4F& color)
{
    auto ins = new DotPrimitive(pos, radius, color);
    ins->autorelease();
    return ins;
}

DotPrimitive::~DotPrimitive()
{
}
