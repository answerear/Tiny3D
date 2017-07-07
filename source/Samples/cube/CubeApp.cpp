

#include "CubeApp.h"
#include <SDL_syswm.h>


using namespace Tiny3D;


CubeApp::CubeApp()
{

}

CubeApp::~CubeApp()
{

}

int32_t CubeApp::go()
{
//     int32_t ret = SampleApp::go();

    Entrance *entrance = new Entrance();
    entrance->setApplicationListener(this);
    
    RenderWindow *renderWindow = nullptr;
    entrance->initialize(false, renderWindow);
    
    Settings &settings = entrance->getConfig();
    Settings renderSettings = settings["Render"].mapValue();

    RenderWindowCreateParam param;
    param._windowLeft = renderSettings["x"].int32Value();
    param._windowTop = renderSettings["y"].int32Value();
    param._windowWidth = renderSettings["Width"].int32Value();
    param._windowHeight = renderSettings["Height"].int32Value();
    param._fullscreen = renderSettings["FullScreen"].boolValue();
    param._colorDepth = renderSettings["ColorDepth"].int32Value();
    param._windowTitle = renderSettings["Title"].stringValue();

    if (!SDL_WasInit(SDL_INIT_VIDEO))
    {
        SDL_InitSubSystem(SDL_INIT_VIDEO);
    }

    mSDLWindow = SDL_CreateWindow(param._windowTitle.c_str(), param._windowLeft, param._windowTop, param._windowWidth, param._windowHeight, SDL_WINDOW_RESIZABLE);

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(mSDLWindow, &wmInfo);

    RenderWindowCreateParamEx paramEx;
    paramEx["renderer"].setLong((long_t)entrance->getActiveRenderer());
    paramEx["MultiSampleQuality"] = renderSettings["MultiSampleQuality"];
    paramEx["vsync"] = renderSettings["VSync"];
    paramEx["externalHandle"] = (long_t)(wmInfo.info.win.window);
    renderWindow = entrance->createRenderWindow(param, paramEx);
    mRenderWindow = renderWindow;
    WindowEventHandler::getInstance().addWindowEventListener(renderWindow, this);
//     Entrance::getInstance().run();
    run();

    delete entrance;

    return 0;
}

bool CubeApp::applicationDidFinishLaunching()
{
    Renderer *renderer = T3D_ENTRANCE.getActiveRenderer();
    renderer->setRenderMode(Renderer::E_RM_WIREFRAME);

    SGNodePtr root = T3D_SCENE_MGR.getRoot();
    
    // 相机变换结点
    SGTransformNodePtr node = SGTransformNode::create();
    root->addChild(node);
    node->lookAt(Vector3(-1.0, 1.0, 2.0), Vector3::ZERO, Vector3::UNIT_Y);

    {
        // 相机结点
        SGCameraPtr camera = SGCamera::create();
        node->addChild(camera);
        camera->setProjectionType(SGCamera::E_PT_PERSPECTIVE);

        Radian fovY(Math::PI * Real(0.5));
        Real ratio = Real(960) / Real(640);
        camera->setPerspective(fovY, ratio, 0.5, 1000.0);

        // 视口
        ViewportPtr viewport = mRenderWindow->addViewport(camera, 0, 0.0, 0.0, 1.0, 1.0);
        viewport->setBackgroundColor(Color4::BLACK);
    }

    // 变换结点
    node = SGTransformNode::create();
    root->addChild(node);

    {
        // 立方体
        SGBoxPtr box = SGBox::create("");
        node->addChild(box);
    }

    return true;
}

void CubeApp::run()
{
    if (!applicationDidFinishLaunching())
        return;

    if (!mSDLWindow)
    {
        // SDL events not initialized
        return;
    }

    while (1)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                Entrance::getInstance().shutdown();
                return;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    //                 mRenderWindow->resize(event.window.data1, event.window.data2);
                    //                 windowResized(mWindow);
                }
                break;
            default:
                //             _fireInputEvent(event);
                break;
            }
        }
        else
        {
            if (!Entrance::getInstance().renderOneFrame())
                break;
        }
    }
    
}
