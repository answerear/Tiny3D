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

#ifndef __T3D_SETTINGS_H__
#define __T3D_SETTINGS_H__


#include "T3DPrerequisites.h"
#include "Resource/T3DResource.h"


namespace Tiny3D
{
    TCLASS()
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
        String  renderHardware = "Direct3D11";

        TRTTI_ENABLE()
    };

    TCLASS()
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

        TRTTI_ENABLE()
    };

    TCLASS()
    struct PluginSettings
    {
        TPROPERTY()
        String  pluginPath = ".";

        TPROPERTY()
        std::list<String> plugins;

        TRTTI_ENABLE()
    };

    TCLASS()
    class T3D_ENGINE_API Settings
    {
        TRTTI_ENABLE()

    public:
        TPROPERTY()
        RenderSettings  renderSettings;

        TPROPERTY()
        LogSettings     logSettings;

        TPROPERTY()
        PluginSettings  pluginSettings;
    };
}


#endif    /*__T3D_SETTINGS_H__*/
