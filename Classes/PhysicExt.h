#pragma once
#include "chipmunk/chipmunk_private.h"
#include <cocos2d.h>
#include <functional>
#include "physics\CCPhysicsWorld.h"

using cocos2d::Vec2;
using cocos2d::Rect;
using cocos2d::PhysicsWorld;
using cocos2d::PhysicsShape;
using cocos2d::PhysicsShapeBox;
using cocos2d::PhysicsShapeCircle;
using cocos2d::PhysicsShapeEdgeBox;
using cocos2d::PhysicsShapeEdgeChain;
using cocos2d::PhysicsShapeEdgePolygon;
using cocos2d::PhysicsShapeEdgeSegment;
using cocos2d::PhysicsShapePolygon;


struct PhysicEx
{
    static PhysicsWorld* instance;
    enum NODE_TAG
    {
        SHADOW_CAST_TAG = 1,
    };

    static void init(PhysicsWorld* pw)
    {
        instance = pw;
    }


	static int OverlapCircle(const Vec2& center, float radius, std::vector<std::pair<int, Vec2*>>& hits)
	{
        Rect rec{center.x - radius, center.y - radius, radius * 2, radius * 2};
        if (instance == nullptr)
        {
            abort();
        }

        UD ud{ hits };
        instance->queryRect(queryFunc, rec, &ud);
        return ud.size;
	}
private:
    struct UD
    {
        std::vector<std::pair<int, Vec2*>>& hits;
        int size;
    };

    static bool queryFunc(PhysicsWorld& world, PhysicsShape& shap, void* ud)
    {
        if (shap.getBody()->getTag() == SHADOW_CAST_TAG || shap.getBody()->getNode()->getTag() == SHADOW_CAST_TAG)
        {
            auto hits = static_cast<UD*>(ud);
            Vec2* points;
            int count = 0;
            auto stype = shap.getType(); 
            switch (stype)
            {
            case PhysicsShape::Type::CIRCLE:
            {
                PhysicsShapeCircle* circle = dynamic_cast<PhysicsShapeCircle*>(&shap);
                auto center = circle->getCenter();
                auto radius = circle->getRadius();
                auto scaleX = circle->getBody()->getNode()->getScaleX();
                auto scaleY = circle->getBody()->getNode()->getScaleY();
                count = 50;
                const float coef = 2.0f * (float)M_PI / count;
                Vec2* vertices = new Vec2[count];
                for (unsigned int i = 0; i < count; i++)
                {
                    float rads = i * coef;
                    GLfloat j = radius * cosf(rads) * scaleX + center.x;
                    GLfloat k = radius * sinf(rads) * scaleY + center.y;

                    vertices[i].x = j;
                    vertices[i].y = k;
                }
                hits->hits.push_back(std::make_pair(count, vertices));
                break;
            }
            case PhysicsShape::Type::BOX:
            case PhysicsShape::Type::POLYGON:
            {
                PhysicsShapePolygon* polygon = dynamic_cast<PhysicsShapePolygon*>(&shap);
                count = polygon->getPointsCount();
                points = new Vec2[count];
                polygon->getPoints(points);
                for (size_t i = 0; i < count; i++)
                {
                    points[i] = shap.getBody()->getNode()->convertToWorldSpace(points[i]);
                }
                hits->hits.push_back(std::make_pair(count, points));
                break;
            }
            case PhysicsShape::Type::EDGESEGMENT:
            {
                PhysicsShapeEdgeSegment* edgesegment = dynamic_cast<PhysicsShapeEdgeSegment*>(&shap);
                count = 2;
                points = new Vec2[count];
                points[0] = edgesegment->getPointA();
                points[1] = edgesegment->getPointB();
                hits->hits.push_back(std::make_pair(count, points));
                break;
            }
            case PhysicsShape::Type::EDGEBOX:
            case PhysicsShape::Type::EDGEPOLYGON:
            {
                PhysicsShapeEdgePolygon* epolygon = dynamic_cast<PhysicsShapeEdgePolygon*>(&shap);
                count = epolygon->getPointsCount();
                points = new Vec2[count];
                epolygon->getPoints(points);
                hits->hits.push_back(std::make_pair(count, points));
                break;
            }
            case PhysicsShape::Type::EDGECHAIN:
            {
                PhysicsShapeEdgeChain* echain = dynamic_cast<PhysicsShapeEdgeChain*>(&shap);
                count = echain->getPointsCount();
                points = new Vec2[count];
                echain->getPoints(points);
                hits->hits.push_back(std::make_pair(count, points));
                break;
            }
            }
            hits->size += count;
            return true;
        }
        return false;
    }
};