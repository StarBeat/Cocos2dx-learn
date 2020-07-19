#include "CCImGuiLayer.h"
#include "imgui.h"
#include "imgui_impl_cocos2dx.h"
#include "CCIMGUI.h"

USING_NS_CC;

void ImGuiLayer::createAndKeepOnTop()
{
    // delay call, once.
    auto director = Director::getInstance();
    CCIMGUI::getInstance();
    
    auto layer = ImGuiLayer::create();
    layer->retain();
    director->setNotificationNode(layer);
   
    if (FileUtils::getInstance()->isFileExist("fonts/fontcn1.ttf"))
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->AddFontFromFileTTF("fonts/fontcn1.ttf", 22.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    }
}

// on "init" you need to initialize your instance
bool ImGuiLayer::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	// init imgui
    setGLProgram(GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_COLOR));

	// events
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event*) -> bool {
        bool inImGuiWidgets = ImGui::IsAnyWindowHovered();
        
        return inImGuiWidgets;
    };
    getEventDispatcher()->addEventListenerWithFixedPriority(listener, -128);
    return true;
}

void ImGuiLayer::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags)
{
    Layer::visit(renderer, parentTransform, parentFlags);
    _command.init(_globalZOrder);
    _command.func = CC_CALLBACK_0(ImGuiLayer::onDraw, this);
    renderer->addCommand(&_command);
}

void ImGuiLayer::onExit()
{
    CCLOGWARN("ImGuiLayer should not exit by director->setNotificationNode");
}

void ImGuiLayer::onDraw()
{
    getGLProgram()->use();

    // create frame
    ImGui_ImplCocos2dx_NewFrame();
    
    // draw all gui
    CCIMGUI::getInstance()->updateImGUI();

    // rendering
    glUseProgram(0);
    
    ImGui::Render();
    
    ImGui_ImplCocos2dx_RenderDrawData(ImGui::GetDrawData());
}
