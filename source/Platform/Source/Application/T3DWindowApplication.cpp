/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
