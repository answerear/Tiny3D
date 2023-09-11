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


#include "T3DNullPlugin.h"
#include "T3DNullRenderer.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    NullPlugin::NullPlugin()
        : mName("NullRenderer")
        , mRenderer(nullptr)
    {

    }

    //--------------------------------------------------------------------------

    NullPlugin::~NullPlugin()
    {

    }

    //--------------------------------------------------------------------------

    const String &NullPlugin::getName() const
    {
        return mName;
    }

    //--------------------------------------------------------------------------

    TResult NullPlugin::install()
    {
        TResult ret = T3D_OK;

        do
        {
            mRenderer = NullRenderer::create();
            if (mRenderer != nullptr)
            {
                ret = T3D_AGENT.addRHIRenderer(mRenderer);
                if (T3D_FAILED(ret))
                {
                    break;
                }
            }
        } while (0);

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult NullPlugin::startup()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult NullPlugin::shutdown()
    {
        TResult ret = T3D_OK;

        return ret;
    }

    //--------------------------------------------------------------------------

    TResult NullPlugin::uninstall()
    {
        TResult ret = T3D_OK;

        do
        {
            ret = T3D_AGENT.removeRHIRenderer(mRenderer);
            if (T3D_FAILED(ret))
            {
                break;
            }

            mRenderer = nullptr;
        } while (0);

        return ret;
    }
}
