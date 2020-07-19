#include "LightDemoScene.h"
#include "PhysicExt.h"
#include "GameManager.h"
#include <CirclePrimitive.h>
#include <extensions\GUI\CCScrollView\CCScrollView.h>
#include <HelloWorldScene.h>
#include <CCIMGUI.h>
#include <Util.h>
using namespace cocos2d;

bool LightDemoScene::init()
{
    //auto closeItem = MenuItemImage::create(
    //    "CloseNormal.png",
    //    "CloseSelected.png",
    //    [this](Ref*) 
    //    {
    //        Image* image = utils::captureNode(this, 1);
    //        //测试看截图
    //        std::string outputFile = FileUtils::getInstance()->getWritablePath() + "test.png";
    //        cocos2d::log("outputFile = %s", outputFile.c_str());
    //        image->saveToFile(outputFile);
    //        CC_SAFE_RELEASE(image);
    //    });
    
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
    this->addChild(_drawNode);

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
    li->setPosition(350, 610);
    li->setLightSize(720, 200);
    layer->addChild(li, 100);

    Light2d* li3 = Light2d::create();
    auto sprite2 = Sprite::create("light.png");
    li3->addChild(sprite2);
    li3->setPosition(900, 150);
    li3->setLightSize(600, 80);
    layer->addChild(li3, 100);
    
    Vec2 points[] = { {554,376}, {727,382}, {730, 549}, {727,382} };
    auto pb4 = PhysicsBody::createEdgeChain(points, sizeof(points) / sizeof(points[0]));
    pb4->setDynamic(false);
    pb4->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    Node* n3 = Node::create();
    layer->addChild(n3);
    n3->addComponent(pb4);

    auto mouse = EventListenerMouse::create();
    mouse->onMouseDown = [&](EventMouse* event)
    {
        _clickHold = true;        
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
        {
            _lightAdd = false;
            _selectedLighttex.clear();
        }
    };
    mouse->onMouseUp = [&](EventMouse* event)
    {
        _clickHold = false;
        if (_lightAdd)
        {
            Light2d* li = Light2d::create();
            auto mi = MenuItemImage::create("light.png", "light.png",
                [li,this](Ref* pSender) {
                    _selectLi = li;
                    _lightAdd = false;
                });
            auto menu = Menu::create(mi, NULL);
   
            menu->setPosition(Vec2::ZERO);
            li->addChild(menu);
            li->setLightSize(900, 50);
            li->setPosition(event->getCursorX(), event->getCursorY());
            this->addChild(li);
        }
        if (_chainAdd)
        {
            if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
            {
                auto pb2 = PhysicsBody::createEdgeChain(_chainPoints.data(), _chainPoints.size());
                pb2->setDynamic(false);
                pb2->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
                Node* n2 = Node::create();
                this->addChild(n2);
                n2->addComponent(pb2);
                _chainPoints.clear();
                _chainAdd = false;
            }
            else
            {
                Vec2 p = { event->getCursorX(), event->getCursorY() };
                _drawNode->drawDot(p, 1, Color4F::GRAY);
                _chainPoints.push_back(p);
            }
        }
        CCLOG("%f,%f\n", event->getCursorX(), event->getCursorY());
    };
    mouse->onMouseMove = [&](EventMouse* event)
    {
        if (_selectLi != nullptr)
        {
            if (_clickHold&& _allowDrag)
            {
                _selectLi->setPosition(event->getCursorX(), event->getCursorY());
            }
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse, this);
    
    CCIMGUI::getInstance()->addImGUI([=]() {
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), u8"单击选中灯光修改属性");
            if (ImGui::Button(u8"允许添加灯光"))
            {
                 _lightAdd= !_lightAdd;
            }
            if (_lightAdd)
            {
                ImGui::Text(u8"点击添加灯光");
            }
            if (ImGui::Button(u8"允许添加阴影"))
            {
                _chainAdd = !_chainAdd;
            }
            if (_chainAdd)
            {
                ImGui::Text(u8"单机添加阴影检测chain的节点，右击结束");
            }
            if (ImGui::Button(u8"清除所有灯光和阴影"))
            {
                auto children = this->getChildren();
                for (const auto& i : children)
                {
                    if (i->getTag() == ::PhysicEx::NODE_TAG::SHADOW_CAST_TAG || i->getTag() == ::PhysicEx::NODE_TAG::LIGHTING_TAG)
                    {
                        this->removeChild(i);
                    }
                }
                _selectLi = nullptr;
            }
            static bool show = true;
            if (_selectLi != nullptr)
            {
                ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
                ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
                ImGui::Begin(u8"灯光设置", &show, ImGuiWindowFlags_NoBackground);// if (ImGui::Begin(u8"灯光设置", &show,ImGuiWindowFlags_NoBackground)) 造成少调用end
                {
                    float x= _selectLi->getPosition().x, y = _selectLi->getPosition().y;
                    ImGui::Checkbox(u8"允许拖动改变位置", &_allowDrag);
                    if (_allowDrag)
                    {
                        ImGui::TextColored(ImVec4(.0f, 1.0f, 0.0f, 1.0f), u8"单击不放拖动改变位置");
                    }
                  //  ImGui::TextColored(ImVec4(.0f, 1.0f, 0.0f, 1.0f), u8"灯光位置x:%f , y: %f", _selectLi->getPosition().x, _selectLi->getPosition().y);
                    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
                    ImGui::DragFloat("Position x", &x, 1.f);
                    ImGui::DragFloat("Position y", &y, 1.f);
                    ImGui::PopStyleColor(1);
                    _selectLi->setPositionX(x);
                    _selectLi->setPositionY(y);
                    ImVec4 clear_color = ImColor(_selectLi->_lighteff->_color.r, _selectLi->_lighteff->_color.g, _selectLi->_lighteff->_color.b);
                    ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::ImColor(0,0,1));
                    ImGui::SliderFloat(u8"常数项衰减系数", &_selectLi->_lighteff->_K0, 0, 2);
                    ImGui::SliderFloat(u8"一阶衰减系数", &_selectLi->_lighteff->_K1, 0, 0.1);
                    ImGui::SliderFloat(u8"二阶项衰减系数", &_selectLi->_lighteff->_K2, 0, 0.01);
                    ImGui::SliderFloat("attenuation", &_selectLi->_lighteff->_attenuation, -2, 2);
                    ImGui::SliderFloat("intensity", &_selectLi->_lighteff->_intensity, 0, 5);
                    ImGui::SliderFloat("lightDistance", &_selectLi->_lighteff->_lightDistance, 0, 2000);
                    ImGui::SliderFloat("lightVolumn",&_selectLi->_lighteff->_lightVolumn, 0, 1000);
                    ImGui::ColorEdit3("color", (float*)&clear_color);
                    _selectLi->_lighteff->_color.r = clear_color.x;
                    _selectLi->_lighteff->_color.g = clear_color.y;
                    _selectLi->_lighteff->_color.b = clear_color.z;

                    ImGui::PopStyleColor(1);
           
                    ImGui::End();
                }
                if (!show)
                {
                    _selectLi = nullptr;
                    show = true;
                }
                
            }
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        }, "LightDemoScene");

    GameManager::Instane()->delayInit();
    auto back = MenuItemLabel::create(Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 24), [](Ref*) {
        Director::getInstance()->replaceScene(HelloWorld::createScene());
        CCIMGUI::getInstance()->removeImGUI("LightDemoScene");
        });
    float x = origin.x + visibleSize.width - back->getContentSize().width / 2;
    float y = origin.y + back->getContentSize().height / 2;
    back->setPosition(Vec2(x, y));
    auto menu = Menu::create(back, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    return true;
}

bool LightDemoScene1::init()
{
    initWithPhysics();
    _physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    GameManager::Instane()->moduleInit(_physicsWorld);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    this->addChild(_drawNode);

    auto mouse = EventListenerMouse::create();
    mouse->onMouseDown = [&](EventMouse* event)
    {
        _clickHold = true;
        if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
        {
            _lightAdd = false;
            _selectedLighttex.clear();
        }
    };
    mouse->onMouseUp = [&](EventMouse* event)
    {
        _clickHold = false;
        if (_lightAdd)
        {
            Light2d* li;
            if (_lightTex)
            {
                if (_selectedLighttex.empty())
                {
                    return;
                }
                li = Light2d::create(_selectedLighttex);
            }
            else
            {
                li = Light2d::create();
            }
            auto mi = MenuItemImage::create("light.png", "light.png",
                [li, this](Ref* pSender) {
                    _selectLi = li;
                    _lightAdd = false;
                });
            auto menu = Menu::create(mi, NULL);

            menu->setPosition(Vec2::ZERO);
            li->addChild(menu);
            li->setLightSize(900, 50);
            li->setPosition(event->getCursorX(), event->getCursorY());
            this->addChild(li);
        }
        if (_chainAdd)
        {
            if (event->getMouseButton() == EventMouse::MouseButton::BUTTON_RIGHT)
            {
                auto pb2 = PhysicsBody::createEdgeChain(_chainPoints.data(), _chainPoints.size());
                pb2->setDynamic(false);
                pb2->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
                Node* n2 = Node::create();
                this->addChild(n2);
                n2->addComponent(pb2);
                _chainPoints.clear();
                _chainAdd = false;
            }
            else
            {
                Vec2 p = { event->getCursorX(), event->getCursorY() };
                _drawNode->drawDot(p, 1, Color4F::GRAY);
                _chainPoints.push_back(p);
            }
        }
        CCLOG("%f,%f\n", event->getCursorX(), event->getCursorY());
    };
    mouse->onMouseMove = [&](EventMouse* event)
    {
        if (_selectLi != nullptr)
        {
            if (_clickHold && _allowDrag)
            {
                _selectLi->setPosition(event->getCursorX(), event->getCursorY());
            }
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouse, this);

    auto sprite = Sprite::create("lightbg1.jpg");
    sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(sprite, 0);

    Light2d* li = Light2d::create();
    auto sprite2 = Sprite::create("light.png");
    li->addChild(sprite2);
    li->setLightSize(900, 50);
    li->setPosition(223, 635);
    this->addChild(li, 100);

    Light2d* li1 = Light2d::create(/*"Lightcookie.png"*/);
    auto sprite3 = Sprite::create("light.png");
    li1->addChild(sprite3);
    li1->setLightSize(500, 200);
    li1->setPosition(500, 80);
    this->addChild(li1, 100);

    Light2d* li3 = Light2d::create("Lightcookie.png");
    auto sprite4 = Sprite::create("light.png");
    li3->addChild(sprite4);
    li3->setLightSize(900, 50);
    li3->setPosition(980, 660);
    this->addChild(li3, 100);

    Vec2 points[] = { {0,428},  {753,428} ,{1080,428} };
    auto pb1 = PhysicsBody::createEdgeChain(points, sizeof(points) / sizeof(points[0]));
    pb1->setDynamic(false);
    pb1->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    Node* n1 = Node::create();
    this->addChild(n1);
    n1->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    n1->addComponent(pb1);
    Vec2 points1[] = { {753,428}/*, {742,434},{727,459}, {738,473}, {723,513}, {735,534}, {722,555}, */,{734,578} };
    auto pb2 = PhysicsBody::createEdgeChain(points1, sizeof(points1) / sizeof(points1[0]));
    pb2->setDynamic(false);
    pb2->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    Node* n2 = Node::create();
    n2->setTag(::PhysicEx::NODE_TAG::SHADOW_CAST_TAG);
    this->addChild(n2);
    n2->addComponent(pb2);

    //auto event = EventListenerKeyboard::create();
    //event->onKeyPressed = [li](EventKeyboard::KeyCode k, Event*)->void
    //{
    //    Vec2 p;
    //    // auto li = this;
    //    if (EventKeyboard::KeyCode::KEY_D == k)
    //    {
    //        p = li->getPosition() + Vec2(40, 0);
    //        li->setPosition(p);
    //    }
    //    if (EventKeyboard::KeyCode::KEY_W == k)
    //    {
    //        p = li->getPosition() + Vec2(0, 40);
    //        li->setPosition(p);
    //    }
    //    if (EventKeyboard::KeyCode::KEY_A == k)
    //    {
    //        p = li->getPosition() + Vec2(-40, 0);
    //        li->setPosition(p);
    //    }
    //    if (EventKeyboard::KeyCode::KEY_S == k)
    //    {
    //        p = li->getPosition() + Vec2(0, -40);
    //        li->setPosition(p);
    //    }
    //};
    //_eventDispatcher->addEventListenerWithSceneGraphPriority(event, this);

    GameManager::Instane()->delayInit();
    CCIMGUI::getInstance()->addImGUI([=]() {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), u8"单击选中灯光修改属性");
        if (ImGui::Button(u8"允许添加点灯光"))
        {
            _lightAdd = !_lightAdd;
            _lightTex = false;
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"允许添加texture灯光"))
        {
            _lightTex = true;
            _lightAdd = !_lightAdd;
        }
        if (_lightAdd)
        {
            ImGui::Text(u8"单击添加灯光，右击结束");
            if (_lightTex)
            {
                auto ls = lsdir(FileUtils::getInstance()->getDefaultResourceRootPath(), "light/*.png");
                static int selected = 0;
                ImGui::BeginChild("left pane", ImVec2(150, 0), true);
                for (int i = 0; i < ls.size(); i++)
                {
                    if (ImGui::Selectable(ls[i].c_str(), selected == i))
                    {
                        selected = i;
                        _selectedLighttex = "light/" + ls[i];
                    }
                }
                ImGui::EndChild();
            }
        }
        if (ImGui::Button(u8"允许添加阴影"))
        {
            _chainAdd = !_chainAdd;
        }
        if (_chainAdd)
        {
            ImGui::Text(u8"单机添加阴影检测chain的节点，右击结束");
        }
        if (ImGui::Button(u8"清除所有灯光和阴影"))
        {
            auto children = this->getChildren();
            for (const auto& i : children)
            {
                if (i->getTag() == ::PhysicEx::NODE_TAG::SHADOW_CAST_TAG || i->getTag() == ::PhysicEx::NODE_TAG::LIGHTING_TAG)
                {
                    this->removeChild(i);
                }
            }
            _selectLi = nullptr;
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
                        sprite->setTexture("BG/" + ls[i]);
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

        static bool show = true;
        if (_selectLi != nullptr)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
            ImGui::Begin(u8"灯光设置", &show, ImGuiWindowFlags_NoBackground);
            float x = _selectLi->getPosition().x, y = _selectLi->getPosition().y;
            ImGui::Checkbox(u8"允许拖动改变位置", &_allowDrag);
            if (_allowDrag)
            {
                ImGui::TextColored(ImVec4(.0f, 1.0f, 0.0f, 1.0f), u8"单击不放拖动改变位置");
            }
            //  ImGui::TextColored(ImVec4(.0f, 1.0f, 0.0f, 1.0f), u8"灯光位置x:%f , y: %f", _selectLi->getPosition().x, _selectLi->getPosition().y);
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(7.0f, 0.6f, 0.6f));
            ImGui::DragFloat("Position x", &x, 1.f);
            ImGui::DragFloat("Position y", &y, 1.f);
            ImGui::PopStyleColor(1);
            _selectLi->setPositionX(x);
            _selectLi->setPositionY(y);
            ImVec4 clear_color = ImColor(_selectLi->_lighteff->_color.r, _selectLi->_lighteff->_color.g, _selectLi->_lighteff->_color.b);
            ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::ImColor(0, 0, 1));
            ImGui::SliderFloat(u8"常数项衰减系数", &_selectLi->_lighteff->_K0, 0, 2);
            ImGui::InputScalar(u8"一阶衰减系数", ImGuiDataType_Float, &_selectLi->_lighteff->_K1, NULL);
            ImGui::InputScalar(u8"二阶项衰减系数", ImGuiDataType_Float, & _selectLi->_lighteff->_K2, NULL);
            ImGui::SliderFloat("attenuation", &_selectLi->_lighteff->_attenuation, -2, 2);
            ImGui::SliderFloat("intensity", &_selectLi->_lighteff->_intensity, 0, 5);
            ImGui::SliderFloat("lightDistance", &_selectLi->_lighteff->_lightDistance, 0, 2000);
            ImGui::SliderFloat("lightVolumn", &_selectLi->_lighteff->_lightVolumn, 0, 1000);
            ImGui::ColorEdit3("color", (float*)&clear_color);
            _selectLi->_lighteff->_color.r = clear_color.x;
            _selectLi->_lighteff->_color.g = clear_color.y;
            _selectLi->_lighteff->_color.b = clear_color.z;
            
            ImGui::PopStyleColor(1);
            
            ImGui::End();
            if (!show)
            {
                _selectLi = nullptr;
                show = true;
            }

        }
      //  ImGui::ShowDemoWindow();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }, "LightDemoScene1");

    auto back = MenuItemLabel::create(Label::createWithTTF("Back", "fonts/Marker Felt.ttf", 24), [](Ref*) {
            Director::getInstance()->replaceScene(HelloWorld::createScene());
            CCIMGUI::getInstance()->removeImGUI("LightDemoScene1");
        });
    float x = origin.x + visibleSize.width - back->getContentSize().width / 2;
    float y = origin.y + back->getContentSize().height / 2;
    back->setPosition(Vec2(x, y));
    auto menu = Menu::create(back, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    return true;
}
