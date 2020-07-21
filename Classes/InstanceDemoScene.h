#pragma once
#include "cocos2d.h"

class InstanceDemoScene : public cocos2d::Scene
{
public:
    virtual bool init();
    ~InstanceDemoScene();
    CREATE_FUNC(InstanceDemoScene);

    int _nnums;
    int _cnums;
};