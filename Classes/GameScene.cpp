#include "GameScene.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "LocalPlayer.h"
#include "PhysicExt.h"
#include "BGEffect.h"

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
    edge->setCollisionBitmask(0);
    this->addComponent(edge);

    _scheduler->scheduleUpdate(GameManager::Instane()->_network, -1, false);
    GameManager::Instane()->delayInit();
    return true;
}
