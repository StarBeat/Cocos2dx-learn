/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "GameManager.h"
#include "PhysicExt.h"
#include "RectPrimitive.h"
#include "CirclePrimitive.h"
#include "DotPrimitive.h"
#include "TrianglePrimitive.h"

#include "BGEffect.h"
#include "Light2d.h"
#include "LightingManager.h"
#include "GameScene.h"
#include "LightDemoScene.h"
#include "InstanceDemoScene.h"
#include "LiquidFunDemo.h"
#include <CCIMGUI.h>
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}


void HelloWorld::onEnter()
{
    Scene::onEnter();
    GameManager::Instane()->moduleInit(_physicsWorld);
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
    initWithPhysics();

  //  _physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    GameManager::Instane()->moduleInit(_physicsWorld);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }
    auto l1 = Label::createWithTTF("RPCAsServer", "fonts/fontcn1.ttf", 24);
    auto l2 = Label::createWithTTF("RPCAsClient", "fonts/Marker Felt.ttf", 24);
    MenuItemLabel* cmenu;
    auto smenu = MenuItemLabel::create(l1, [](Ref*)
        {
            GameManager::Instane()->asServer();
            auto scene = GameScene::create();
            auto rescene = CCTransitionCrossFade::create(1, scene);
            Director::getInstance()->replaceScene(rescene);
        });
    smenu->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - l1->getContentSize().height));
    cmenu = MenuItemLabel::create(l2, [](Ref* r)
        {
            GameManager::Instane()->asClient();
            auto scene = GameScene::create();
            auto rescene = CCTransitionCrossFade::create(3, scene);
            Director::getInstance()->replaceScene(rescene);
        });
    cmenu->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - l1->getContentSize().height - l2->getContentSize().height));
    
    auto demo = MenuItemLabel::create(Label::createWithTTF("LightDemoScene", "fonts/Marker Felt.ttf", 24), [](Ref*) {
            auto scene = LightDemoScene::create();
           // auto rescene = CCTransitionProgressHorizontal::create(1, scene);
            auto rescene = CCTransitionCrossFade::create(1, scene);
            Director::getInstance()->replaceScene(rescene);
        });
    demo->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 3 * l2->getContentSize().height));
    auto demo1 = MenuItemLabel::create(Label::createWithTTF("LightDemoScene1", "fonts/Marker Felt.ttf", 24), [](Ref*) {
        auto scene = LightDemoScene1::create();
        // auto rescene = CCTransitionProgressHorizontal::create(1, scene);
        auto rescene = CCTransitionCrossFade::create(1, scene);
        Director::getInstance()->replaceScene(rescene);
        });
    demo1->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 4 * l2->getContentSize().height));
    auto demo2 = MenuItemLabel::create(Label::createWithTTF("InstanceDemoScene", "fonts/Marker Felt.ttf", 24), [](Ref*) {
        auto scene = InstanceDemoScene::create();
        auto rescene = CCTransitionCrossFade::create(1, scene);
        //Director::getInstance()->pushScene(rescene);
        Director::getInstance()->replaceScene(rescene);
        });
    demo2->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 5 * l2->getContentSize().height));
    auto demo3 = MenuItemLabel::create(Label::createWithTTF("LiquidFunDemo", "fonts/Marker Felt.ttf", 24), [](Ref*) {
        auto scene = LiquidFunDemo::create();
        auto rescene = CCTransitionCrossFade::create(1, scene);
        Director::getInstance()->replaceScene(rescene);
        });
    demo3->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 6 * l2->getContentSize().height));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, smenu, cmenu, demo, demo1, demo2, demo3, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    auto pb2 = PhysicsBody::createBox(l1->getContentSize());
    pb2->setDynamic(false);
    pb2->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    l1->addComponent(pb2);


    GameManager::Instane()->delayInit(this);
  //  auto bg = BGEffect::create(8);
 ///*   bg->setTexturei(2, "snail.png");
 //   bg->setTexturei(3, "snai2.png");
 //   bg->setTexturei(4, "snai3.png");*/
 //   bg->genBindBuffer();
 //   this->addChild(bg);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("BG.png");
    if (sprite == nullptr)
    {
        problemLoading("'HelloWorld.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x + 100, visibleSize.height/2 + origin.y + 100));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }
    Light2d* li2 = Light2d::create();
    li2->setPosition(cmenu->getPosition() + Vec2{0,50});
    li2->setLightSize(200, 80);
    this->addChild(li2, 100);

    Light2d* li = Light2d::create();
    li->setPosition(750,160);
    li->setLightSize(720, 720);
    this->addChild(li, 100);

    Light2d* li3 = Light2d::create();
    li3->setPosition(435,150);
    li3->setLightSize(200, 80);
    this->addChild(li3, 100);

    auto ac1 = MoveBy::create(3, { 1050, 460 });

    Light2d* li4 = Light2d::create();
    li4->setPosition(435, 150);
    li4->setLightSize(720, 200);
    this->addChild(li4, 100);
    li4->runAction(ac1->reverse());

    Light2d* li5 = Light2d::create();
    li5->setPosition(435, 150);
    li5->setLightSize(300, 150);
    this->addChild(li5, 100);
    //DotPrimitive* d = DotPrimitive::create({0,0}, 3, Color4F::WHITE);
    //li5->addChild(d);
    li5->runAction(ac1);

    auto event = EventListenerKeyboard::create();
    event->onKeyPressed = [li](EventKeyboard::KeyCode k, Event*)->void
        {
            Vec2 p;
           // auto li = this;
            if (EventKeyboard::KeyCode::KEY_D == k)
            {
                for (size_t i = 0; i < 50; i++)
                {
                    DotPrimitive* d = DotPrimitive::create({0,0}, 3, Color4F::WHITE);
                    auto a2 = CallFunc::create([d] { d->removeFromParent();}); 
                    auto seq = Sequence::create(DelayTime::create(0.1), a2, NULL);
                    li->addChild(d);
                    d->runAction(seq);
                }

                p = li->getPosition() + Vec2(40, 0);
                li->setPosition(p);
            }
            if (EventKeyboard::KeyCode::KEY_W == k)
            {
                p = li->getPosition() + Vec2(0, 40);
                li->setPosition(p);
            }
            if (EventKeyboard::KeyCode::KEY_A == k)
            {
                p = li->getPosition() + Vec2(-40, 0);
                li->setPosition(p);
            }
            if (EventKeyboard::KeyCode::KEY_S == k)
            {
                p = li->getPosition() + Vec2(0, -40);
                li->setPosition(p);
            }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(event, this);

    auto pb3 = PhysicsBody::createCircle(30);
    pb3->setDynamic(false);
    pb3->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    Node* n2 = Node::create();
    this->addChild(n2);
    n2->addComponent(pb3);
    n2->setPosition(1050, 460);

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);

}
//RectPrimitive* rect1 = RectPrimitive::create({ 0,0 }, { 50,50 }, Color4F::RED, 1, Color4F::GREEN);
//
//this->addChild(rect1);
//rect1->setPosition(300, 300);
//auto pb1 = PhysicsBody::createBox({ 50,50 });
//pb1->setDynamic(false);
//pb1->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
//rect1->addComponent(pb1);

//RectPrimitive* rect2 = RectPrimitive::create({ 0,0 }, { 50,50 }, Color4F::RED, 1, Color4F::GREEN);
//this->addChild(rect2);

//CirclePrimitive* c = CirclePrimitive::create({0,0}, 50, 50, 1, 1, Color4F::GREEN, 10, Color4F::YELLOW);
//c->setPosition(200, 100);
//this->addChild(c);

//DotPrimitive* d = DotPrimitive::create({ 60,70 }, 10, Color4F::YELLOW);
//this->addChild(d);

//for (size_t i = 0; i < 1000; i++)
//{
//    auto node = CirclePrimitive::create({ 0,0 }, 5, 50, 1, 1, Color4F::GREEN, 1.5, Color4F::YELLOW);
//    node->setPosition(i*10 % (int)visibleSize.width, i*5 % (int)visibleSize.height);
//    this->addChild(node);
//}
void HelloWorld::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
}
