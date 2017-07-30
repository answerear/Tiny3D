/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#ifndef __T3D_RENDER_WINDOW_H__
#define __T3D_RENDER_WINDOW_H__


#include "T3DPrerequisites.h"
#include "Misc/T3DCommon.h"
#include "Render/T3DRenderTarget.h"


namespace Tiny3D
{
    /** Manages the target rendering window.
     * @remarks
     *  This class handles a window into which the contents of a scene are
     *  rendered. There is a many-to-1 relationship between instances of this
     *  class an instance of Renderer which controls the rendering of the scene.
     *  There may be more than one window in the case of level editor tools etc.
     *  This class is abstract since there may be different implementations for
     *  different windowing systems.
     * @remarks
     *  Instances are created and communicated with by the render system
     *  although client programs can get a reference to it from the renderer if
     *  required for resizing or moving. Note that you can have multiple
     *  viewpoints in the window for effects like rear-view mirrors and
     *  picture-in-picture views (see Viewport and Camera).
     */
    class T3D_ENGINE_API RenderWindow : public RenderTarget
    {
    public:
        /** Default constructor. */
        RenderWindow();
        /** Destructor. */
        virtual ~RenderWindow();

        virtual void update();

        /**
         * @brief Creates & displays the new window.
         * @param [in] rkCreateParam
         *      Parameter for creating rendering window.
         * @param [in] rkCreateParamEx
         *      A variable number of pointers to platform-specific arguments.
         *      The actual requirements must be defined by the implementing
         *      subclasses.
         * @return return true if succeeded.
         */
        virtual bool create(
            const String &name,
            const RenderWindowCreateParam &rkCreateParams,
            const RenderWindowCreateParamEx &rkCreateParamEx) = 0;

        /** Destroys the window. */
        virtual void destroy() = 0;

        /**
         * @brief Swaps the frame buffers to display the next frame.
         * @remarks
         *      All render windows are double-buffered so that no 'in-progress'
         *      versions of the scene are displayed during rendering.
         *      Once rendering has completed (to an off-screen version of
         *      the window) the buffers are swapped to display the new frame.
         */
        virtual void swapBuffers() = 0;

        virtual bool isFullScreen() const;

        virtual void windowMovedOrResized();
    };
}


#endif  /*__T3D_RENDER_WINDOW_H__*/
