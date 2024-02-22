/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#include "Adapter/Common/T3DSDLApplication.h"
#include "T3DPlatformErrorDef.h"
#include "T3DApplication.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    static int AppEventWatcher(void *userdata, SDL_Event *event)
    {
        switch (event->type)
        {
        case SDL_APP_DIDENTERBACKGROUND:
            {
                T3D_APPLICATION.applicationDidEnterBackground();
            }
            break;
        case SDL_APP_WILLENTERFOREGROUND:
            {
                T3D_APPLICATION.applicationWillEnterForeground();
            }
            break;
        case SDL_APP_TERMINATING:
            {
                T3D_APPLICATION.applicationWillTerminate();
            }
            break;
        case SDL_APP_LOWMEMORY:
            {
                T3D_APPLICATION.applicationLowMemory();
            }
            break;
        }
        
        return 0;
    }

    //--------------------------------------------------------------------------

    SDLApplication::SDLApplication()
    {

    }

    //--------------------------------------------------------------------------

    SDLApplication::~SDLApplication()
    {

    }

    //--------------------------------------------------------------------------

    TResult SDLApplication::init()
    {
        TResult ret = T3D_ERR_FAIL;

        do 
        {
            if (SDL_Init(0) != 0)
            {
                const char *error = SDL_GetError();
                break;
            }

            if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0)
            {
                const char *error = SDL_GetError();
                break;
            }
            
            SDL_AddEventWatch(AppEventWatcher, nullptr);

            ret = T3D_OK;
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool SDLApplication::pollEvents()
    {
        bool ret = true;
        SDL_Event ev;

        while (SDL_PollEvent(&ev) != 0)
        {
            switch (ev.type)
            {
            case SDL_QUIT:
                {
                    ret = false;
                }
                break;
            }
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void SDLApplication::release()
    {
        SDL_Quit();
    }

    //--------------------------------------------------------------------------

    void *SDLApplication::getNativeAppObject()
    {
        return nullptr;
    }
}

