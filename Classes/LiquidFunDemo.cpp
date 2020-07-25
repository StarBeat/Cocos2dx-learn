#include "LiquidFunDemo.h"
#include "GameManager.h"
#include "PhysicSprite.h"
#include "PuddingSprite.h"
#include "BGEffect.h"
#include <MetaBallSprite.h>
#include "ParticleEffect.h"
#include "b2Physic.h"

USING_NS_CC;
#define PTM_RATIO B2Physic::PTM_RATIO
const static unsigned int Edge_chain = 1 << 2 | B2Physic::UD_TAG;

bool LiquidFunDemo::init()
{
    memset(&_bslot, 0, sizeof(_bslot) / sizeof(_bslot[0]));
    _lhold = false;

    ImGui::CreateContext();
    initWithPhysics();
    _physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    GameManager::Instane()->moduleInit(_physicsWorld);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto s = visibleSize;
    auto origin = Director::getInstance()->getVisibleOrigin();
    auto _world = GameManager::Instane()->_b2world;

    _drawNode = DrawNode::create();
    this->addChild(_drawNode, 1);
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

    if (ldata.load())
    {
        sp->setTexture(ldata.bg);
        if (ldata.water_count > 0)
        {
            for (auto& p : ldata.waters)
            {
                ParticleEffectSpawn* pew = ParticleEffectSpawn::create();
                pew->setType(ParticleEffectSpawn::Type::WATER);
                pew->setPosition(p);
                pew->setLife(10);
                this->addChild(pew);
            }
        }
        if (ldata.laya_count > 0)
        {
            for (auto& p : ldata.layas)
            {
                ParticleEffectSpawn* pe = ParticleEffectSpawn::create();
                pe->setPosition(p);
                pe->setType(ParticleEffectSpawn::Type::LAVA);
                pe->setLife(15);
                this->addChild(pe);
            }
        }
        if (ldata.chain_count > 0)
        {
            int offset = 0;
            for (size_t i = 0; i < ldata.chain_count; i++)
            {
                b2BodyDef def;
                def.position.Set(0, 0);
                def.type = b2_staticBody;
                b2Body* bd = _world->CreateBody(&def);

                b2ChainShape shap;
                shap.CreateChain(&ldata.chains[offset], ldata.chain_steps[i]);

                // Define the dynamic body fixture.
                b2FixtureDef fixtureDef;
                fixtureDef.shape = &shap;
                fixtureDef.density = 1.0f;
                fixtureDef.friction = 0.3f;
                //fixtureDef.isSensor = true;
                bd->CreateFixture(&fixtureDef);
                bd->SetUserData((void*)(Edge_chain));
                offset += ldata.chain_steps[i];
            }
            for (size_t i = 0; i < ldata.chain_point_count; i++)
            {
                _drawNode->drawDot(Vec2(ldata.chains[i]) * PTM_RATIO,1.5, Color4F::GREEN);
            }
        }
    }

    GameManager::Instane()->delayInit(this);

    auto mouse = EventListenerMouse::create();
    mouse->onMouseDown = [=](EventMouse* event)
    {
        Vec2 p(event->getCursorX(), event->getCursorY());
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
        {
            _lhold = true;
            if (_bslot[0])
            {
                ParticleEffectSpawn* pew = ParticleEffectSpawn::create();
                pew->setType(ParticleEffectSpawn::Type::WATER);
                pew->setPosition(p);
                this->addChild(pew);
                ++ldata.water_count;
                ldata.waters.push_back(p);
            }
            if (_bslot[1])
            {
                ParticleEffectSpawn* pe = ParticleEffectSpawn::create();
                pe->setPosition(p);
                pe->setType(ParticleEffectSpawn::Type::LAVA);
                pe->setLife(10);
                this->addChild(pe);
                ++ldata.laya_count;
                ldata.layas.push_back(p);
            }
            if (_bslot[2])
            {
                if (_sselect.empty())
                {
                    return;
                }
                auto pp = PuddingSprite::create(_sselect); //"Images/r2.png"
                pp->initParticlePosition(p.x, p.y);
                this->addChild(pp);
            }
            if (_bslot[3])
            {
                _path.push_back({ p.x / PTM_RATIO, p.y / PTM_RATIO });
                _drawNode->drawDot({ event->getCursorX()  , event->getCursorY() }, 1.5, Color4F::GREEN);
            }
        }
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
        {
            _sselect.clear();
            if (_bslot[3])
            {
                _path.clear();
            }
            memset(&_bslot, 0, sizeof(_bslot) / sizeof(_bslot[0]));
         //   addNewSpriteAtPosition({ event->getCursorX(), event->getCursorY() });
        }
       
        CCLOG("%f,%f\n", event->getCursorX(), event->getCursorY());
    };
    mouse->onMouseMove = [=](EventMouse* event) 
    {
        if (_bslot[3] && _lhold)
        {
            _drawNode->drawDot({ event->getCursorX()  , event->getCursorY() }, 1.5, Color4F::GREEN);
            _path.push_back({ event->getCursorX() / PTM_RATIO , event->getCursorY() / PTM_RATIO });
        }
      //  CCLOG("%f,%f\n", event->getCursorX(), event->getCursorY());
    };
    mouse->onMouseUp = [=](EventMouse* event) 
    {
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
        {
            _lhold = false;
        }

        if (_bslot[3])
        {
            if (!_path.empty())
            {
                b2BodyDef def;
                def.position.Set(0, 0); 
                def.type = b2_staticBody;
                b2Body* bd = _world->CreateBody(&def);

                b2ChainShape shap;
                shap.CreateChain(&_path[0], _path.size());

                // Define the dynamic body fixture.
                b2FixtureDef fixtureDef;
                fixtureDef.shape = &shap;
                fixtureDef.density = 1.0f;
                fixtureDef.friction = 0.3f;
                //fixtureDef.isSensor = true;
                bd->CreateFixture(&fixtureDef); 
                bd->SetUserData((void*)(Edge_chain));

                ++ldata.chain_count;
                ldata.chain_point_count += _path.size();
                ldata.chain_steps.push_back(_path.size());
                ldata.chains.insert(ldata.chains.end(), _path.begin(), _path.end());
                _path.clear();
            }
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse, this);

    //auto mp = Sprite::create("ParticleImage.png");
    //mp->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    //this->addChild(mp);
    //mp->setScaleX(0.15);
    //mp->setScaleY(0.15);
    //mp->setColor(Color3B::RED);

    
    CCIMGUI::getInstance()->addImGUI([=]() {
      //  ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), u8"单击选中灯光修改属性");
        ImGui::Checkbox(u8"允许添加water发射点", &_bslot[0]);
        if (_bslot[0])
        {
            ImGui::Text(u8"单击添加，右击结束");
            memset(&_bslot, 0, sizeof(_bslot) / sizeof(_bslot[0]));
            _bslot[0] = true;
        }
        ImGui::Checkbox(u8"允许添加laya发射点", &_bslot[1]);
        if (_bslot[1])
        {
            ImGui::Text(u8"单击添加，右击结束");
            memset(&_bslot, 0, sizeof(_bslot) / sizeof(_bslot[0]));
            _bslot[1] = true;
        }
        ImGui::Checkbox(u8"允许添加布丁", &_bslot[2]);
        if (_bslot[2])
        {
            ImGui::Text(u8"单击添加，右击结束");
            auto ls = lsdir(FileUtils::getInstance()->getDefaultResourceRootPath(), "Images/*.png");
            static int selected = 0;
            ImGui::BeginChild("left pane", ImVec2(150, 0), true);
            for (int i = 0; i < ls.size(); i++)
            {
                if (ImGui::Selectable(ls[i].c_str(), selected == i))
                {
                    selected = i;
                    _sselect = "Images/" + ls[i];
                }
                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    Texture2D* texture = Director::getInstance()->getTextureCache()->addImage("Images/" + ls[i]);
                    ImGui::Text(u8"预览");
                    ImGui::Image((ImTextureID)(intptr_t)texture->getName(), ImVec2(300, 400), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
                    ImGui::EndTooltip();
                }
            }
            ImGui::EndChild();
            memset(&_bslot, 0, sizeof(_bslot) / sizeof(_bslot[0]));
            _bslot[2] = true;
        }
        ImGui::Checkbox(u8"允许添加chain", &_bslot[3]);
        if (_bslot[3])
        {
            memset(&_bslot, 0, sizeof(_bslot) / sizeof(_bslot[0]));
            _bslot[3] = true;
            ImGui::Text(u8"单击添加,拖动鼠标添加，左键抬起结束");
        }
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
                        ldata.bg = "BG/" + ls[i];
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

        if (ImGui::Button(u8"清除所有节点"))
        {
            sp->retain();
            _drawNode->retain();
            GameManager::Instane()->_b2physic->retain();
            this->removeAllChildren();
            this->addChild(sp);
            this->addChild(_drawNode);
            this->addChild(GameManager::Instane()->_b2physic);
            Director::getInstance()->getScheduler()->scheduleUpdate(GameManager::Instane()->_b2physic, 0, true);
            GameManager::Instane()->_b2physic->resume();
            auto bdls = _world->GetBodyList();
            auto bdcount = _world->GetBodyCount();
            auto tmp = bdls;
            do 
            {
                if ((unsigned int)tmp->GetUserData() & B2Physic::UD_TAG)
                {
                    if((unsigned int)tmp->GetUserData() == Edge_chain)
                    _world->DestroyBody(tmp);
                }
                tmp = bdls->GetNext();

            } while (--bdcount);
            ldata.clear();
        }
        if (ImGui::Button(u8"清除chain绘制节点"))
        {
            _drawNode->clear();
        }
        static bool _b2debug = false;
        ImGui::Checkbox(u8"开启box2d debug", &_b2debug);
        if (_b2debug)
        {
            uint32 flags = 0;
            static bool b[6] = { 0 };
            ImGui::Checkbox(u8"shap", &b[0]);
            ImGui::Checkbox(u8"joint", &b[1]);
            ImGui::Checkbox(u8"aabb", &b[2]);
            ImGui::Checkbox(u8"pair", &b[3]);
            ImGui::Checkbox(u8"Centroid", &b[4]);
            ImGui::Checkbox(u8"particle", &b[5]);
            if (b[0])
            {
                flags |= b2Draw::e_shapeBit;
            }
            for (size_t i = 1; i < 6; i++)
            {
                flags |= (1 << i) * b[i];
            }

            GameManager::Instane()->_b2physic->setDebugFlag(flags);
        }
        else
        {
            GameManager::Instane()->_b2physic->setDebugFlag(0);
        }
        //ImGui::Text(u8"背景旋转");
        //float x = sp->getRotation3D().x, y = sp->getRotation3D().y, z = sp->getRotation3D().z;
        //ImGui::SliderFloat("x", &x, 0, 360);
        //ImGui::SliderFloat("y", &y, 0, 360);
        //ImGui::SliderFloat("z", &z, 0, 360);
        //sp->setRotation3D({ x,y,z });
        if (ImGui::Button(u8"保存当前场景"))
        {
            if (ldata.bg.empty())
            {
                ldata.bg = "HelloWorld.png";
            }
            ldata.save();
        }
        //float sx = mp->getScaleX(), sy = mp->getScaleY();
        //ImGui::SliderFloat("sx", &sx, 0, 2);
        //ImGui::SliderFloat("sy", &sy, 0, 2);
        //mp->setScaleX(sx);
        //mp->setScaleY(sy);

      //  ImGui::ShowDemoWindow();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
       
 }, "LiquidFunDemo");
    return true;
}

void LiquidFunDemo::addNewSpriteAtPosition(Vec2 p)
{
    CCLOG("Add sprite %0.2f x %02.f", p.x, p.y);

    auto sprite = PhysicSprite::create("Images/r1.png");
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
    body->SetGravityScale(0);
    sprite->setB2Body(body);
    sprite->setPTMRatio(PTM_RATIO);
}