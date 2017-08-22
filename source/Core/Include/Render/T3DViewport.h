/***************************************************************************************************
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
 **************************************************************************************************/

#ifndef __T3D_VIEWPORT_H__
#define __T3D_VIEWPORT_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Misc/T3DColor4.h"
#include "Misc/T3DObject.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Viewport : public Object
    {
    public:
        static ViewportPtr create(SGCamera *camera, RenderTarget *target, 
            Real left, Real top, Real width, Real height, int32_t nZOrder);

        virtual ~Viewport();

        void update();

        SGCamera *getCamera() const;
        void setCamera(SGCamera *camera);

        RenderTarget *getRenderTarget() const;

        Real getLeft() const;
        Real getTop() const;
        Real getWidth() const;
        Real getHeight() const;

        int32_t getActualLeft() const;
        int32_t getActualTop() const;
        int32_t getActualWidth() const;
        int32_t getActualHeight() const;

        const Matrix4 &getViewportMatrix() const;

        void setDimensions(Real left, Real top, Real width, Real height);

        void setBackgroundColor(const Color4 &rkColor);
        const Color4 &getBackgroundColor() const;

    protected:
        Viewport(SGCamera *camera, RenderTarget *target, 
            Real left, Real top, Real width, Real height, int32_t nZOrder);

        void updateDimemsions();

    protected:
        SGCamera        *mCamera; /// Camera binding this camera
        RenderTarget    *mTarget; /// Render target binding this view port

        Real    mLeft;
        Real    mTop;
        Real    mWidth;
        Real    mHeight;

        int32_t mActualLeft;
        int32_t mActualTop;
        int32_t mActualWidth;
        int32_t mActualHeight;

        Color4  mBkgndColor;

        Matrix4 mMatrix;        /// ÊÓ¿Ú±ä»»¾ØÕó
    };
}


#include "T3DViewport.inl"


#endif  /*__T3D_VIEWPORT_H__*/
