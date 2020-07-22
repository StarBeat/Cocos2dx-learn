#include "GameScene.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "LocalPlayer.h"
#include "PhysicExt.h"
#include "BGEffect.h"
#include <CirclePrimitive.h>
#include <DotPrimitive.h>
#include <HelloWorldScene.h>
#include "AsteroidManager.h"

using namespace cocos2d;
GameScene* GameScene::gameScene = nullptr;

bool GameScene::init()
{
    if (!Scene::init())
    {
        return false;
    }
    gameScene = this;
    initWithPhysics();

 //   _physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    GameManager::Instane()->moduleInit(_physicsWorld);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    gameSize = visibleSize * 4;
    gameRect = Rect(Vec2(-visibleSize.width, -visibleSize.height), visibleSize);

    auto bg = BGEffect::create();
    bg->genBindBuffer();
    this->addChild(bg);

    auto layer = Layer::create();
    this->addChild(layer);

    auto e = DrawNode::create();
    e->drawRect(Vec2(-visibleSize.width, -visibleSize.height), Vec2(visibleSize), Color4F::GRAY);
    layer->addChild(e);

    _scheduler->scheduleUpdate(GameManager::Instane()->_network, -1, false);
    GameManager::Instane()->delayInit(this);

    //auto b = CirclePrimitive::create(Vec2(100, 100), 20, 30, 1, 1, Color4F::GRAY);
    //auto pb = PhysicsBody::createCircle(20);
    //pb->setTag(::PhysicEx::NODE_TAG::ASTEROID_TAG);
    ////pb->setContactTestBitmask(::PhysicEx::NODE_BIT::LPLAYER_BIT | ::PhysicEx::NODE_BIT::NPLAYER_BIT);
    //pb->setCategoryBitmask(::PhysicEx::NODE_BIT::ASTEROID_BIT);
    //pb->setCollisionBitmask(0);
    //b->addComponent(pb);
    //this->addChild(b);

    auto mouse = EventListenerMouse::create();
    mouse->onMouseDown = [](EventMouse* event)
    {
        CCLOG("%f,%f\n", event->getCursorX(), event->getCursorY());
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse, this);
    auto back = MenuItemLabel::create(Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 24), [&](Ref*) {
        _scheduler->unscheduleAllForTarget(GameManager::Instane()->_network);
        _scheduler->unscheduleAllForTarget(PlayerManager::Instane());
        AsteroidManager::Instane()->stop();
        Director::getInstance()->replaceScene(HelloWorld::createScene());
        gameScene = nullptr;
        });
    float x = origin.x + visibleSize.width - back->getContentSize().width / 2;
    float y = origin.y + back->getContentSize().height / 2;
    back->setPosition(Vec2(x, y));
    auto menu = Menu::create(back, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    return true;
}
