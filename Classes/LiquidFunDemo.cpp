#include "LiquidFunDemo.h"
#include "GameManager.h"
#include <Box2D\Box2D.h>
#include "LFSpriteNode.h"
#include "PuddingSprite.h"

USING_NS_CC;
#define PTM_RATIO B2Physic::PTM_RATIO
bool LiquidFunDemo::init()
{
    memset(&_bslot, 0, sizeof(_bslot) / sizeof(_bslot[0]));
    ImGui::CreateContext();
    initWithPhysics();
    _physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    GameManager::Instane()->moduleInit(_physicsWorld);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto s = visibleSize;
    auto origin = Director::getInstance()->getVisibleOrigin();
    auto _world = GameManager::Instane()->_b2world;
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, 0); // bottom-left corner
    b2Body* groundBody = _world->CreateBody(&groundBodyDef);

    // Define the ground box shape.
    b2EdgeShape groundBox;

    // bottom
    groundBox.Set(b2Vec2(0 / PTM_RATIO, 0 / PTM_RATIO), b2Vec2(s.width / PTM_RATIO, 0 / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    // top
    groundBox.Set(b2Vec2(0 / PTM_RATIO, s.height / PTM_RATIO), b2Vec2(s.width / PTM_RATIO, s.height / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    // left
    groundBox.Set(b2Vec2(0 / PTM_RATIO, 0 / PTM_RATIO), b2Vec2(0 / PTM_RATIO, s.height / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    // right
    groundBox.Set(b2Vec2(s.width / PTM_RATIO, 0 / PTM_RATIO), b2Vec2(s.width / PTM_RATIO, s.height / PTM_RATIO));
    groundBody->CreateFixture(&groundBox, 0);

    auto sp = Sprite::create("HelloWorld.png");
    sp->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(sp);

 /*   auto pp = PuddingSprite::create("Images/r2.png");
    pp->initParticlePosition(visibleSize.width / 2 + origin.x , visibleSize.height / 2 + origin.y);
    this->addChild(pp);*/

    auto color = LayerColor::create(Color4B::GRAY);
    this->addChild(color, -1);
    GameManager::Instane()->delayInit(this);

    auto mouse = EventListenerMouse::create();
    mouse->onMouseDown = [=](EventMouse* event)
    {
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
        {
            addNewSpriteAtPosition({ event->getCursorX(), event->getCursorY() });
        }
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
        {
            auto pp = PuddingSprite::create("Images/r2.png");
            pp->initParticlePosition(event->getCursorX(), event->getCursorY());
            //pp->setScale(0.5);
            this->addChild(pp);
        }
        CCLOG("%f,%f\n", event->getCursorX(), event->getCursorY());
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse, this);

    CCIMGUI::getInstance()->addImGUI([=]() {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), u8"单击选中灯光修改属性");
        
        static bool _allowBg = false;
        ImGui::Checkbox(u8"允许改变背景", &_allowBg);
        {
            if (_allowBg)
            {
                auto ls = lsdir(FileUtils::getInstance()->getDefaultResourceRootPath(), "BG/*.png");
                static int selected = 0;
                ImGui::BeginChild("left pane", ImVec2(150, 0), true);
                for (int i = 0; i < ls.size(); i++)
                {
                    if (ImGui::Selectable(ls[i].c_str(), selected == i))
                    {
                        selected = i;
                        sp->setTexture("BG/" + ls[i]);
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("BG/" + ls[i]);
                        ImGui::Text(u8"预览");
                        ImGui::Image((ImTextureID)(intptr_t)texture->getName(), ImVec2(300, 400), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                        ImGui::EndTooltip();
                    }
                }
                ImGui::EndChild();
                ImGui::SameLine();
                if (selected != -1)
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), u8"当前选中: %s", ls[selected].c_str());
                }
            }
            ImGui::NewLine();
        }
        
        ImGui::ShowDemoWindow();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }, "LiquidFunDemo");
    return true;
}

void LiquidFunDemo::addNewSpriteAtPosition(Vec2 p)
{
    CCLOG("Add sprite %0.2f x %02.f", p.x, p.y);

    auto sprite = LFSpriteNode::create("Images/r1.png");
    this->addChild(sprite);

    // Define the dynamic body.
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(p.x / PTM_RATIO, p.y / PTM_RATIO);

    b2Body* body = GameManager::Instane()->_b2world->CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2CircleShape dynamicCircle;
    dynamicCircle.m_radius = sprite->getContentSize().width / 2 / PTM_RATIO;

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicCircle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    //fixtureDef.isSensor = true;
    body->CreateFixture(&fixtureDef);

    sprite->setB2Body(body);
    sprite->setPTMRatio(PTM_RATIO);
}