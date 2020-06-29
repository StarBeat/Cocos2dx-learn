#pragma once
#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    virtual bool init() override;

    CREATE_FUNC(GameScene);
public:
    cocos2d::Vec2 gameSize;
    static GameScene* gameScene;
};

