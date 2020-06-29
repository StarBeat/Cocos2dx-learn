#pragma once
#include "cocos2d.h"
#include "PrimitiveEffect.h"
#include "InstanceRenderCommand.h"

using cocos2d::Vec2;
using cocos2d::Mat4; 
using cocos2d::RenderQueue;
using cocos2d::Color4B;
using cocos2d::Tex2F;
using cocos2d::V2F_C4B_T2F;
using cocos2d::V2F_C4B_T2F_Triangle; 
using cocos2d::Renderer; 
using cocos2d::MATRIX_STACK_TYPE;

static inline Vec2 v2f(float x, float y)
{
    Vec2 ret(x, y);
    return ret;
}

static inline Vec2 v2fadd(const Vec2& v0, const Vec2& v1)
{
    return v2f(v0.x + v1.x, v0.y + v1.y);
}

static inline Vec2 v2fsub(const Vec2& v0, const Vec2& v1)
{
    return v2f(v0.x - v1.x, v0.y - v1.y);
}

static inline Vec2 v2fmult(const Vec2& v, float s)
{
    return v2f(v.x * s, v.y * s);
}

static inline Vec2 v2fperp(const Vec2& p0)
{
    return v2f(-p0.y, p0.x);
}

static inline Vec2 v2fneg(const Vec2& p0)
{
    return v2f(-p0.x, -p0.y);
}

static inline float v2fdot(const Vec2& p0, const Vec2& p1)
{
    return  p0.x * p1.x + p0.y * p1.y;
}

static inline Vec2 v2fnormalize(const Vec2& p)
{
    Vec2 r(p.x, p.y);
    r.normalize();
    return v2f(r.x, r.y);
}

static inline Vec2 __v2f(const Vec2& v)
{
    //#ifdef __LP64__
    return v2f(v.x, v.y);
    // #else
    //     return * ((Vec2*) &v);
    // #endif
}

static inline Tex2F __t(const Vec2& v)
{
    return *(Tex2F*)&v;
}


using cocos2d::Node;

struct IPrimitive : public cocos2d::Node
{
    IPrimitive() 
    {
    }
    virtual ~IPrimitive() 
    {
        _instancecommande.rmPrimitive(this);
        glDeleteBuffers(1, &_vbo);
        glDeleteVertexArrays(1, &_vao);
        delete _trianglesbuffer;
    }
    void calculateTriangleDone()
    {
        _instancecommande.addPrimitive(this);
    }
	unsigned int _vertex_count;
    cocos2d::Mat4 _mvp;
    cocos2d::V2F_C4B_T2F* _trianglesbuffer;
    virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
    {
        _mvp = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION) * transform;
      //  _instancecommande.pushMvp(_primitiveHash, _mvp);
        _instancecommande.init(RenderQueue::QUEUE_GROUP::GLOBALZ_ZERO, const_cast<Mat4&>(transform), flags, _primitiveHash);
        renderer->addCommand(&_instancecommande);
    }
    void prepared()
    {
    }

    unsigned int _primitiveHash;
protected:
    GLuint _vao;
    GLuint _vbo;
    InstanceRenderCommand _instancecommande;
protected:
    void createPolygonTriangle(const cocos2d::Vec2* verts, int count, const cocos2d::Color4F& fillColor, float borderWidth, const cocos2d::Color4F& borderColor)
	{
		bool outline = (borderColor.a > 0.0 && borderWidth > 0.0);

		auto  triangle_count = outline ? (3 * count - 2) : (count - 2);
		_vertex_count = 3 * triangle_count;
        
        _trianglesbuffer = new V2F_C4B_T2F[_vertex_count];
        V2F_C4B_T2F_Triangle* cursor = (V2F_C4B_T2F_Triangle*)_trianglesbuffer;

        for (int i = 0; i < count - 2; i++)
        {
           V2F_C4B_T2F_Triangle tmp = {
                {verts[0], Color4B(fillColor), *reinterpret_cast<Tex2F*>(&Vec2(0,0))},
                {verts[i + 1], Color4B(fillColor), *reinterpret_cast<Tex2F*>(&Vec2(0,0))},
                {verts[i + 2], Color4B(fillColor), *reinterpret_cast<Tex2F*>(&Vec2(0,0))},
            };

            *cursor++ = tmp;
        }
        if (outline)
        {
            struct ExtrudeVerts { cocos2d::Vec2 offset, n; };
            struct ExtrudeVerts* extrude = (struct ExtrudeVerts*)malloc(sizeof(struct ExtrudeVerts) * count);
            memset(extrude, 0, sizeof(struct ExtrudeVerts) * count);

            for (int i = 0; i < count; i++)
            {
                cocos2d::Vec2 v0 = __v2f(verts[(i - 1 + count) % count]);
                cocos2d::Vec2 v1 = __v2f(verts[i]);
                cocos2d::Vec2 v2 = __v2f(verts[(i + 1) % count]);

                cocos2d::Vec2 n1 = v2fnormalize(v2fperp(v2fsub(v1, v0)));
                cocos2d::Vec2 n2 = v2fnormalize(v2fperp(v2fsub(v2, v1)));

                cocos2d::Vec2 offset = v2fmult(v2fadd(n1, n2), 1.0 / (v2fdot(n1, n2) + 1.0));
                struct ExtrudeVerts tmp = { offset, n2 };
                extrude[i] = tmp;
            }

            for (int i = 0; i < count; i++)
            {
                int j = (i + 1) % count;
                cocos2d::Vec2 v0 = __v2f(verts[i]);
                cocos2d::Vec2 v1 = __v2f(verts[j]);

                cocos2d::Vec2 n0 = extrude[i].n;

                cocos2d::Vec2 offset0 = extrude[i].offset;
                cocos2d::Vec2 offset1 = extrude[j].offset;

                cocos2d::Vec2 inner0 = v2fsub(v0, v2fmult(offset0, borderWidth));
                cocos2d::Vec2 inner1 = v2fsub(v1, v2fmult(offset1, borderWidth));
                cocos2d::Vec2 outer0 = v2fadd(v0, v2fmult(offset0, borderWidth));
                cocos2d::Vec2 outer1 = v2fadd(v1, v2fmult(offset1, borderWidth));

                cocos2d::V2F_C4B_T2F_Triangle tmp1 = {
                    {inner0, Color4B(borderColor), __t(v2fneg(n0))},
                    {inner1, Color4B(borderColor), __t(v2fneg(n0))},
                    {outer1, Color4B(borderColor), __t(n0)}
                };
                *cursor++ = tmp1;

                cocos2d::V2F_C4B_T2F_Triangle tmp2 = {
                    {inner0, Color4B(borderColor), __t(v2fneg(n0))},
                    {outer0, Color4B(borderColor), __t(n0)},
                    {outer1, Color4B(borderColor), __t(n0)}
                };
                *cursor++ = tmp2;
            }

            free(extrude);
        }
    }
};