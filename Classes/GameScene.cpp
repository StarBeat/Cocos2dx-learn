#include "GameScene.h"
#include "GameManager.h"
#include "PlayerManager.h"
#include "LocalPlayer.h"

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
    gameSize = Vec2(4096, 4096);
    GameManager::Instane()->moduleInit(_physicsWorld);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    _scheduler->scheduleUpdate(GameManager::Instane()->_network, -1, false);
	return true;
}
