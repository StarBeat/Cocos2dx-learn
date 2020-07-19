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
    _nums = 1000;
    ImGui::CreateContext();
    initWithPhysics();
    _physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    GameManager::Instane()->moduleInit(_physicsWorld);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    std::vector<Color4F> colors;
    for (size_t i = 0; i < 30; i++)
    {
        colors.push_back(Color4F(Random::Instane()->get<float>(0, 1), Random::Instane()->get<float>(0, 1), Random::Instane()->get<float>(0, 1), Random::Instane()->get<float>(0, 1)));
    }
    
    auto mouse = EventListenerMouse::create();
    mouse->onMouseUp = [=](EventMouse* event)
    {
        auto root = Layer::create();
        this->addChild(root);
        root->setPosition(event->getCursorX(), event->getCursorY());
        for (size_t i = 0; i < this->_nums; i++)
        {
            auto color = colors[Random::Instane()->get<int>(0, 30)];
            auto node = CirclePrimitive::create({ 0,0 }, 2, 50, 1, 1, color);
            //  node->setPosition(Random::Instane()->get<int>(0, visibleSize.width), Random::Instane()->get<int>(0, visibleSize.height));
            root->addChild(node);
            auto node1 = RectPrimitive::create({}, Vec2(2, 2), color);
            // node1->setPosition(Random::Instane()->get<int>(0, visibleSize.width), Random::Instane()->get<int>(0, visibleSize.height));
            root->addChild(node1);
        }
        const auto a2 = CallFunc::create([root, visibleSize] {
            root->setVisible(true);
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
                auto a2 = CallFunc::create([i] {i->removeFromParent(); });
                auto seq = Sequence::create(DelayTime::create(Random::Instane()->get<float>(0.5, 3)), a2, NULL);
                auto parll = Spawn::create(a, seq, NULL);
                i->runAction(parll);
            }
            });
        root->runAction(a2);
        CCLOG("%f,%f\n", event->getCursorX(), event->getCursorY());
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse, this);

    auto back = MenuItemLabel::create(Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 24), [](Ref*) {
        Director::getInstance()->replaceScene(HelloWorld::createScene());
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
            ImGui::SetWindowSize({ 350, 150});
            ImGui::Text("点击生成图元"U8.c_str());

            ImGui::SliderInt(u8"图元数量", &_nums, 50, 20000);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
 
        }, "InstanceDemoScene");
    GameManager::Instane()->delayInit();

    return true;
}

InstanceDemoScene::~InstanceDemoScene()
{
    CCIMGUI::getInstance()->removeImGUI("InstanceDemoScene");
}
