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

#ifndef __T3D_SETTINGS_H__
#define __T3D_SETTINGS_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    TSTRUCT()
    struct RenderSettings
    {
        TPROPERTY()
        bool    fullscreen = false;   /// 是否全屏

        TPROPERTY()
        int32_t width = 1280;

        TPROPERTY()
        int32_t height = 720;

        TPROPERTY()
        int32_t x = 100;

        TPROPERTY()
        int32_t y = 100;

        TPROPERTY()
        int32_t colorDepth = 32;

        TPROPERTY()
        String  title = "Tiny3D Sample";

        TPROPERTY()
        int32_t MSAA = 16;

        TPROPERTY()
        bool    vsync = false;

        TPROPERTY()
        String  renderer = "Direct3D11";

        TPROPERTY()
        String  iconPath = "";
    };

    TSTRUCT()
    struct LogSettings
    {
        TPROPERTY()
        int32_t appID = 1000;

        TPROPERTY()
        String  tag = "Tiny3D";

        TPROPERTY()
        String level = "DEBUG";

        TPROPERTY()
        int32_t expired = 7;

        TPROPERTY()
        int32_t cacheSize = 50;

        TPROPERTY()
        int32_t cacheTime = 10000;
    };

    TSTRUCT()
    struct PluginSettings
    {
        TPROPERTY()
        String  pluginPath = ".";

        TPROPERTY()
        TList<String> plugins;
    };

    using ComponentUpdateOrders = TList<String>;
    
    TSTRUCT()
    struct ComponentSettings
    {
        TPROPERTY()
        ComponentUpdateOrders updateOrders;

        ComponentSettings()
        {
            updateOrders.emplace_back("Transform3D");
            updateOrders.emplace_back("Camera");
            updateOrders.emplace_back("Geometry");
        }
    };

    TSTRUCT()
    struct T3D_ENGINE_API Settings
    {
        TPROPERTY()
        RenderSettings  renderSettings {};

        TPROPERTY()
        LogSettings     logSettings {};

        TPROPERTY()
        PluginSettings  pluginSettings {};

        TPROPERTY()
        ComponentSettings   componentSettins {};
    };
}


#endif    /*__T3D_SETTINGS_H__*/
