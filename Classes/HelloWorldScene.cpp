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
#include "Light2d.h"
#include "LightingManager.h"

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

    _physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

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

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    //auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    //if (label == nullptr)
    //{
    //    problemLoading("'fonts/Marker Felt.ttf'");
    //}
    //else
    //{
    //    // position the label on the center of the screen
    //    label->setPosition(Vec2(origin.x + visibleSize.width/2,
    //                            origin.y + visibleSize.height - label->getContentSize().height));

    //    // add the label as a child to this layer
    //    this->addChild(label, 1);
    //}

    // add "HelloWorld" splash screen"
    //auto sprite = Sprite::create("BG.png");
    //if (sprite == nullptr)
    //{
    //    problemLoading("'HelloWorld.png'");
    //}
    //else
    //{
    //    // position the sprite on the center of the screen
    //    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    //    // add the sprite as a child to this layer
    //    this->addChild(sprite, 0);
    //}

    //RectPrimitive* rect = RectPrimitive::create({ 0,0 }, { 50,505 }, Color4F::GRAY, 1, Color4F::GREEN);
    //
    //this->addChild(rect);
    //auto pb = PhysicsBody::createBox({50,50});
    //pb->setDynamic(false);
    //pb->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    //rect->addComponent(pb);
    

    RectPrimitive* rect1 = RectPrimitive::create({ 0,0 }, { 50,50 }, Color4F::GRAY, 1, Color4F::GREEN);
    
    this->addChild(rect1);
    rect1->setPosition(300, 300);
    auto pb1 = PhysicsBody::createBox({ 50,50 });
    pb1->setDynamic(false);
    pb1->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    rect1->addComponent(pb1);

    Light2d* li = Light2d::create();
    li->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(li, 100);

    auto event = EventListenerKeyboard::create();
    event->onKeyPressed = [li](EventKeyboard::KeyCode k, Event*)->void
        {
            Vec2 p;
            if (EventKeyboard::KeyCode::KEY_D == k)
            {
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
    
 /*   auto pb1 = PhysicsBody::createCircle(50);
    pb1->setDynamic(false);
    pb1->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    li->addComponent(pb1);*/
   


   /* Light2d* li2 = Light2d::create();
    li2->setPosition({0,0});
    */
    /*std::vector<std::pair<int, Vec2*>> hits;
    int i = ::PhysicEx::OverlapCircle({0,0}, 1000, hits);
    for (auto j : hits)
    {
        for (size_t k = 0; k < j.first; k++)
        {
            CCLOG("hits %f, %f \n", j.second[k].x, j.second[k].y);
        }
        delete j.second;
    }*/


  /*  auto dn = DrawNode::create();
    this->addChild(dn);
    dn->drawSolidCircle({ 50,50 }, 50, 0, 360, Color4F::GREEN);*/

    GameManager::Instane()->delayInit();
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

void HelloWorld::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags)
{
}
