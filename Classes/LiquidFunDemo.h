#pragma once
#include "cocos2d.h"
#include <CCIMGUI.h>
#include <Util.h>
#include "Box2D\Box2D.h"

class LiquidFunDemo : public cocos2d::Scene
{
public:
    virtual bool init();
    void addNewSpriteAtPosition(cocos2d::Vec2 p);
    CREATE_FUNC(LiquidFunDemo);
    cocos2d::DrawNode* _drawNode;

    bool _bslot[5];
    bool _lhold;
    std::string _sselect;
    std::vector<b2Vec2> _path;
};