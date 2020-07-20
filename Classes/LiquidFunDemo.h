#pragma once
#include "cocos2d.h"
#include <CCIMGUI.h>
#include <Util.h>

class LiquidFunDemo : public cocos2d::Scene
{
public:
    virtual bool init();
    void addNewSpriteAtPosition(cocos2d::Vec2 p);
    CREATE_FUNC(LiquidFunDemo);

    bool _bslot[5];
};