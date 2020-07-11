#pragma once
#include "cocos2d.h"
#include "GUI/CCScrollView/CCScrollView.h"

class LightDemoScene : public cocos2d::Scene
{
public:
    virtual bool init();

    CREATE_FUNC(LightDemoScene);
    cocos2d::extension::ScrollView* scroll;

};