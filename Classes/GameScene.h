#pragma once
#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    virtual bool init() override;

    CREATE_FUNC(GameScene);
public:
    cocos2d::Vec2 gameSize;
    cocos2d::Rect gameRect;
    static GameScene* gameScene;
};

