#include "InstanceDemoScene.h"
#include <CirclePrimitive.h>
#include "RectPrimitive.h"
#include "Random.h"
#include "HelloWorldScene.h"
#include <Light2d.h>
#include "GameManager.h"
#include <math.h>
#include "imgui.h"
#include <CCIMGUI.h>
#include "Util.h"
using namespace cocos2d;

bool InstanceDemoScene::init()
{
    _nnums = 1000;
    _cnums = 10;
    ImGui::CreateContext();
    initWithPhysics();
    _physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    GameManager::Instane()->moduleInit(_physicsWorld);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    static std::vector<Color4F> colors;
    for (size_t i = 0; i < _cnums; i++)
    {
        colors.push_back(Color4F(Random::Instane()->get<float>(0, 1), Random::Instane()->get<float>(0, 1), Random::Instane()->get<float>(0, 1), Random::Instane()->get<float>(0, 1)));
    }
    
    auto mouse = EventListenerMouse::create();
    mouse->onMouseUp = [=](EventMouse* event)
    {
        auto root = Layer::create();
        this->addChild(root);
        root->setPosition(event->getCursorX(), event->getCursorY());
        for (size_t i = 0; i < _nnums; i++)
        {
            auto color = colors[Random::Instane()->get<int>(0, colors.size() - 1)];
            auto node = CirclePrimitive::create({ 0,0 }, 2, 5, 1, 1, color);
            //  node->setPosition(Random::Instane()->get<int>(0, visibleSize.width), Random::Instane()->get<int>(0, visibleSize.height));
            root->addChild(node);
            auto node1 = RectPrimitive::create({}, Vec2(2, 2), color);
            // node1->setPosition(Random::Instane()->get<int>(0, visibleSize.width), Random::Instane()->get<int>(0, visibleSize.height));
            root->addChild(node1);
        }
        auto children = root->getChildren();
        for (const auto& i : children)
        {
            auto pos = Vec2(Random::Instane()->get<int>(-visibleSize.width, visibleSize.width),
                Random::Instane()->get<int>(-visibleSize.width, visibleSize.width))+ Vec2(100,100);
            if (pos.getLength() > visibleSize.width)
                {
                    if (Random::Instane()->get<int>(0, 2) == 0)
                    {
                        pos.x = 0;
                    }
                    else
                    {
                        pos.y = 0;
                    }
                }
            auto a = MoveBy::create(3, pos);
            auto a2 = CallFunc::create([i] { i->removeFromParent(); }); // 在release下会出现奇怪的渲染问题 /*i->removeFromParent();*/ 
            auto seq = Sequence::create(DelayTime::create(Random::Instane()->get<float>(0.5, 3)), a2, NULL);
            auto parll = Spawn::create(a, seq, NULL);
            i->runAction(parll);
        }
        CCLOG("%f,%f\n", event->getCursorX(), event->getCursorY());
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse, this);

    auto back = MenuItemLabel::create(Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 24), [=](Ref*) {
        this->stopAllActions();
         //Director::getInstance()->popScene();
        Director::getInstance()->replaceScene(HelloWorld::createScene());
        CCIMGUI::getInstance()->removeImGUI("InstanceDemoScene");
        });
    float x = origin.x + visibleSize.width - back->getContentSize().width / 2;
    float y = origin.y + back->getContentSize().height / 2;
    back->setPosition(Vec2(x, y));
    auto menu = Menu::create(back, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    static ImVec4 clear_color = ImColor(114, 144, 154);
    CCIMGUI::getInstance()->addImGUI([=]() {
        {
            ImGui::SetWindowSize({ 350, 200});
            ImGui::Text("点击生成图元"U8.c_str());

            ImGui::SliderInt(u8"图元数量", &_nnums, 50, 20000);
            ImGui::NewLine();
    
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        }, "InstanceDemoScene");
    GameManager::Instane()->delayInit(this);

    return true;
}

InstanceDemoScene::~InstanceDemoScene()
{
}
