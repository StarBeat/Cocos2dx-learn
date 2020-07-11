#include "LightDemoScene.h"
#include "Light2d.h"
#include "PhysicExt.h"
#include "GameManager.h"
#include <CirclePrimitive.h>
#include <extensions\GUI\CCScrollView\CCScrollView.h>

using namespace cocos2d;

bool LightDemoScene::init()
{
    //auto visibleSize = Director::getInstance()->getVisibleSize();
    //auto origin = Director::getInstance()->getVisibleOrigin();

    //auto closeItem = MenuItemImage::create(
    //    "CloseNormal.png",
    //    "CloseSelected.png",
    //    [this](Ref*) 
    //    {
    //        Image* image = utils::captureNode(this, 1);
    //        //²âÊÔ¿´½ØÍ¼
    //        std::string outputFile = FileUtils::getInstance()->getWritablePath() + "test.png";
    //        cocos2d::log("outputFile = %s", outputFile.c_str());
    //        image->saveToFile(outputFile);
    //        CC_SAFE_RELEASE(image);
    //    });

    //closeItem->setPosition(origin + Vec2(visibleSize) - Vec2(closeItem->getContentSize() / 2));

    //// create menu, it's an autorelease object
    //auto menu = Menu::create(closeItem, nullptr);
    //menu->setPosition(Vec2::ZERO);
    //this->addChild(menu, 1);


    //test
    //auto bg = LayerColor::create(Color4B(128, 128, 128, 255), 320, 220);
    //bg->setAnchorPoint(Vec2(0.5, 0.5));
    //bg->setIgnoreAnchorPointForPosition(false);
    //bg->setPosition(Vec2(visibleSize / 2));
    //this->addChild(bg);
    //auto scroll = cocos2d::extension::ScrollView::create(Size(300, 200));
    //scroll->setAnchorPoint(Vec2(0.5, 0.5));
    //scroll->setIgnoreAnchorPointForPosition(false);
    //scroll->setPosition(Vec2(bg->getContentSize() / 2) + origin);
    //bg->addChild(scroll);
    //auto spriteItem = Sprite::create("HelloWorld.png");
    //spriteItem->setAnchorPoint(Vec2(0, 0));
    //scroll->addChild(spriteItem);
    //this->scroll = scroll;

    initWithPhysics();
    _physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);

    GameManager::Instane()->moduleInit(_physicsWorld);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto layer = Layer::create();
    this->addChild(layer);

    auto sprite = Sprite::create("HelloWorld.png");
    sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x + 100, visibleSize.height / 2 + origin.y + 100));
    layer->addChild(sprite, 0);
    
    Light2d* li = Light2d::create();
    auto sprite1 = Sprite::create("light.png");
    li->addChild(sprite1);
    li->setPosition(750, 160);
    li->setLightSize(720, 200);
    layer->addChild(li, 100);

    Light2d* li3 = Light2d::create();
    auto sprite2 = Sprite::create("light.png");
    li3->addChild(sprite2);
    li3->setPosition(900, 150);
    li3->setLightSize(600, 80);
    layer->addChild(li3, 100);

    //auto pb3 = PhysicsBody::createCircle(30);
    //pb3->setDynamic(false);
    //pb3->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    //Node* n2 = Node::create();
    //this->addChild(n2);
    //n2->addComponent(pb3);
    //n2->setPosition(1050, 460);
    
    Vec2 points[] = { {554,376}, {727,382}, {730, 549}, {727,382} };
    auto pb4 = PhysicsBody::createEdgeChain(points, sizeof(points) / sizeof(points[0]));
    pb4->setDynamic(false);
    pb4->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    Node* n3 = Node::create();
    layer->addChild(n3);
    n3->addComponent(pb4);
   // n3->setPosition(sprite->getPosition());

 /*   auto pb5 = PhysicsBody::createEdgeSegment(Vec2(0, 0), sprite->getPosition());
    pb5->setDynamic(false);
    pb5->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    Node* n5 = Node::create();
    this->addChild(n5);
    n5->addComponent(pb5);*/

    auto mouse = EventListenerMouse::create();
    mouse->onMouseDown = [](EventMouse* event)
    {
        CCLOG("%f,%f\n", event->getCursorX(), event->getCursorY());
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse, this);
    auto event = EventListenerKeyboard::create();
    event->onKeyPressed = [li](EventKeyboard::KeyCode k, Event*)->void
    {
        Vec2 p;
        // auto li = this;
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

    for (size_t i = 0; i < 20000; i++)
    {
        auto node = CirclePrimitive::create({ 0,0 }, 5, 50, 1, 1, Color4F::GREEN, 1.5, Color4F::YELLOW);
        node->setPosition(i*10 % (int)visibleSize.width, i*5 % (int)visibleSize.height);
        layer->addChild(node);
    }

    GameManager::Instane()->delayInit();
    return true;
}
