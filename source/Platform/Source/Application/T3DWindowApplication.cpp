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


#include "Application/T3DWindowApplication.h"
#include "T3DPlatform.h"
#include "Adapter/T3DFactoryInterface.h"
#include "Adapter/T3DApplicationInterface.h"
#include "T3DPlatformErrorDef.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    WindowApplication::WindowApplication()
    {
        mApp = T3D_PLATFORM_FACTORY.createPlatformApplication();
    }

    //--------------------------------------------------------------------------

    WindowApplication::~WindowApplication()
    {
        T3D_SAFE_DELETE(mApp);
    }

    //--------------------------------------------------------------------------

    TResult WindowApplication::init()
    {
        TResult ret = T3D_ERR_FAIL;

        do 
        {
            if (mApp == nullptr)
            {
                ret = T3D_ERR_INVALID_POINTER;
                break;
            }

            ret = mApp->init();
            if (T3D_FAILED(ret))
                break;

            ret = Application::init();
        } while (false);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool WindowApplication::pollEvents()
    {
        bool ret = false;

        if (mApp != nullptr)
        {
            ret = mApp->pollEvents();
        }

        if (ret)
        {
            // 系统平台层轮询
            T3D_PLATFORM.poll();
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void WindowApplication::release()
    {
        if (mApp != nullptr)
        {
            mApp->release();
        }
    }

    //--------------------------------------------------------------------------

    void *WindowApplication::getNativeAppObject() const
    {
        if (mApp != nullptr)
        {
            return mApp->getNativeAppObject();
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------
}
