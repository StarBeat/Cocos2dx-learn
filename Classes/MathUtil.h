#pragma once
#include <math.h>
#include "cocos2d.h"
using cocos2d::Vec2;
using cocos2d::Vec3;

struct MathUtilEx
{
public:
    static inline Vec3 Rotate(Vec3 v, float rad)
    {
        auto rx = cosf(rad);
        auto ry = sinf(rad);
        // (vx + vy i) * (rx + ry i)
        return Vec3(rx * v.x - ry * v.y, rx * v.y + ry * v.x, 0);
    }
    static inline Vec3 Cross(const Vec3& lhs, const Vec3& rhs)
    {
        return Vec3(
            lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.z * rhs.x - lhs.x * rhs.z,
            lhs.x * rhs.y - lhs.y * rhs.x);
    }
    static inline float Dot(const Vec3& lhs, const Vec3& rhs)
    { 
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z; 
    }
    static inline Vec3 Inverse(const Vec3& inVec)
    {
        return Vec3(1.0F / inVec.x, 1.0F / inVec.y, 1.0F / inVec.z);
    }
    static inline float Magnitude(const Vec3& inV)
    {
        return sqrtf(Dot(inV, inV)); 
    }
    static inline Vec2 ToVec2(const Vec3& in)
    {
        return Vec2(in.x, in.y);
    }
    static Vec3 Normalize(const Vec3& in)
    {
        float n = in.x * in.x + in.y * in.y + in.z * in.z;
        Vec3 out;
        // Already normalized.
        if (n == 1.0f)
            return in;

        n = std::sqrt(n);

        n = 1.0f / n;
        out.x = n * in.x;
        out.y = n * in.y;
        out.z = n * in.z;
        return out;
    }
};