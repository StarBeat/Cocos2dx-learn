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

#include "AppDelegate.h"
#include "HelloWorldScene.h"

// #define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif
#include <CCImGuiLayer.h>
#include <CCIMGUI.h>

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}
WNDPROC OldWndProc;
LRESULT winProc(UINT message, WPARAM wParam, LPARAM lParam, BOOL* pProcessed)
{
    const int captionHeight = 10;
    const int frameWidth = 10;
    static POINTS cursor;
    switch (message)
    {
    case WM_NCHITTEST:
    {
        RECT  rc;
        GetClientRect(Director::getInstance()->getOpenGLView()->getWin32Window(), &rc);
        int w = rc.right - rc.left;
        int h = rc.bottom - rc.top;
        POINT point;
        GetCursorPos(&point);
        ScreenToClient(Director::getInstance()->getOpenGLView()->getWin32Window(), &point);
        rc = { frameWidth, captionHeight, w - frameWidth - frameWidth, h - captionHeight - frameWidth };
        RECT rectTopLeft = { 0, 0, frameWidth, captionHeight };
        RECT rectTop = { frameWidth, 0, w - frameWidth, captionHeight };
        RECT rectTopRight = { w - frameWidth, 0, w, captionHeight };
        RECT rectLeft = { 0, captionHeight, frameWidth, h - captionHeight - frameWidth };
        RECT rectRight = { w - frameWidth, captionHeight, w, h - captionHeight - frameWidth };
        RECT rectBottm = { frameWidth, h - frameWidth, w - frameWidth, h };
        RECT rectBottmLeft = { 0, h - frameWidth, frameWidth, h };
        RECT rectBottmRight = { w - frameWidth, h - frameWidth, w, h };


        if (PtInRect(&rc, point))
        {
            OutputDebugString(L"CLicent\n");
            return HTCLIENT;
        }
        else if (PtInRect(&rectTopLeft, point))
        {
            OutputDebugString(L"TopLeft\n");
            return HTTOPLEFT;
        }
        else if (PtInRect(&rectTop, point))
        {
            OutputDebugString(L"Caption\n");
            return HTCAPTION;
        }
        else if (PtInRect(&rectTopRight, point))
        {
            OutputDebugString(L"TopRight\n");
            return HTTOPRIGHT;
        }
        else if (PtInRect(&rectLeft, point))
        {
            OutputDebugString(L"Left\n");
            return HTLEFT;
        }
        else if (PtInRect(&rectRight, point))
        {
            OutputDebugString(L"Right\n");
            return HTRIGHT;
        }
        else if (PtInRect(&rectBottm, point))
        {
            OutputDebugString(L"Bottom\n");
            return HTBOTTOM;
        }
        else if (PtInRect(&rectBottmLeft, point))
        {
            OutputDebugString(L"BottomLeft\n");
            return HTBOTTOMLEFT;
        }
        else if (PtInRect(&rectBottmRight, point))
        {
            OutputDebugString(L"BottomRight\n");
            return HTBOTTOMRIGHT;
        }
    }
    break;
    case WM_LBUTTONDOWN:
    {
        OutputDebugString(L"WM_LBUTTONDOWN\n");
        cursor = MAKEPOINTS(lParam);
    }
    break;
    case WM_LBUTTONUP:
    {
        OutputDebugString(L"WM_LBUTTONUP\n");
        POINTS pts = MAKEPOINTS(lParam);
        int xDaly = pts.x - cursor.x;
        int yDaly = pts.y - cursor.y;
        RECT rectClient;
        GetWindowRect(Director::getInstance()->getOpenGLView()->getWin32Window(), &rectClient);
        int w = rectClient.right - rectClient.left;
        int h = rectClient.bottom - rectClient.top;
        MoveWindow(Director::getInstance()->getOpenGLView()->getWin32Window(), rectClient.left + xDaly, rectClient.top + yDaly, w, h, true);
    }
    break;
    default:
        break;
    }
        DefWindowProc(Director::getInstance()->getOpenGLView()->getWin32Window(), message, wParam, lParam);
    return CallWindowProc((WNDPROC)OldWndProc, Director::getInstance()->getOpenGLView()->getWin32Window(), message, wParam, lParam);
}
HHOOK _gMsgHook;
LRESULT CALLBACK hook_proc(int code, WPARAM w, LPARAM l)
{
    MSG* ms = (MSG*)l;

    if (code == HC_ACTION)//Checks if an action is beeing active
    {

        CWPSTRUCT* pwp = (CWPSTRUCT*)l;
        if (pwp->message == WM_INITDIALOG)
            OldWndProc = (WNDPROC)SetWindowLong(pwp->hwnd, GWL_WNDPROC, (LONG)winProc);
    }
    return CallNextHookEx(_gMsgHook, code, w, l);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        
        glview = GLViewImpl::createWithRect("Demo", Rect{0,0, designResolutionSize.width, designResolutionSize.height }, 1, true);
        //glfwWindowHint(GLFW_DECORATED, GL_FALSE);
        //DWORD dskPid = 0;
        //DWORD tid = ::GetWindowThreadProcessId(glview->getWin32Window(), &dskPid);
        //_gMsgHook = ::SetWindowsHookEx(WH_CALLWNDPROC, hook_proc, NULL, tid);
#else
        glview = GLViewImpl::create("Demo");
#endif
        director->setOpenGLView(glview);
    }
    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);
    ImGuiLayer::createAndKeepOnTop();
    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
    SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}
