#pragma once

#include "cocos2d.h"

USING_NS_CC;

class ImGuiLayer : public cocos2d::Layer
{
public:
    bool detached = false;

    static void createAndKeepOnTop();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init() override;

    virtual void visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags) override;
    void onExit() override;
    void onDraw();

    // implement the "static create()" method manually
    CREATE_FUNC(ImGuiLayer);

private:
    CustomCommand _command;
};
