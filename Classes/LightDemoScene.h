#pragma once
#include "cocos2d.h"
#include "GUI/CCScrollView/CCScrollView.h"
#include "Light2d.h"

struct LightDemoHelper
{
    bool _lightAdd;
    bool _lightTex;
    bool _chainAdd;
    bool _allowDrag;
    bool _clickHold;
    std::string _selectedLighttex;
    Light2d* _selectLi;
    std::vector<cocos2d::Vec2> _chainPoints;
    cocos2d::DrawNode* _drawNode;
    LightDemoHelper()
    {
        _drawNode = cocos2d::DrawNode::create();
        _lightAdd = false;
        _chainAdd = false;
        _lightTex = false;
        _clickHold = false;
        _allowDrag = false;
        _selectLi = nullptr;
    }
};
class LightDemoScene : public cocos2d::Scene, public LightDemoHelper
{
public:
    virtual bool init();

    CREATE_FUNC(LightDemoScene);

};
class LightDemoScene1 : public cocos2d::Scene, public LightDemoHelper
{
public:
    virtual bool init();

    CREATE_FUNC(LightDemoScene1);
};