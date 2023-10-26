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

#ifndef __T3D_CAMERA_H__
#define __T3D_CAMERA_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Render/T3DRenderWindow.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Camera : public Object
    {
    public:
        static CameraPtr create();

        RenderWindowPtr getRenderWindow() const { return mRenderWindow; }
        
    protected:
        Camera() = default;

        ~Camera() override = default;

        /// 相机所属的渲染窗口
        RenderWindowPtr mRenderWindow {nullptr};
    };
}


#endif    /*__T3D_CAMERA_H__*/
