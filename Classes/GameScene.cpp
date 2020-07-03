#include "GameScene.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "LocalPlayer.h"
#include "PhysicExt.h"
#include "BGEffect.h"
#include <CirclePrimitive.h>
#include <DotPrimitive.h>

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

    _physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    GameManager::Instane()->moduleInit(_physicsWorld);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    gameSize = visibleSize * 4;

    auto bg = BGEffect::create();
    bg->genBindBuffer();
    this->addChild(bg);

    auto edge = PhysicsBody::createEdgeBox(Size(gameSize));
    edge->setContactTestBitmask(0xffffffff);
    edge->setTag(::PhysicEx::NODE_TAG::GAMEEDGE_TAG);
    edge->setCollisionBitmask(0xffffffff);
    this->addComponent(edge);

    _scheduler->scheduleUpdate(GameManager::Instane()->_network, -1, false);
    GameManager::Instane()->delayInit();

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
    return true;
}
