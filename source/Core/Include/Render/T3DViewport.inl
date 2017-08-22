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

namespace Tiny3D
{
    inline SGCamera *Viewport::getCamera() const
    {
        return mCamera;
    }

    inline void Viewport::setCamera(SGCamera *camera)
    {
        mCamera = camera;
    }

    inline RenderTarget *Viewport::getRenderTarget() const
    {
        return mTarget;
    }

    inline Real Viewport::getLeft() const
    {
        return mLeft;
    }

    inline Real Viewport::getTop() const
    {
        return mTop;
    }

    inline Real Viewport::getWidth() const
    {
        return mWidth;
    }

    inline Real Viewport::getHeight() const
    {
        return mHeight;
    }

    inline int32_t Viewport::getActualLeft() const
    {
        return mActualLeft;
    }

    inline int32_t Viewport::getActualTop() const
    {
        return mActualTop;
    }

    inline int32_t Viewport::getActualWidth() const
    {
        return mActualWidth;
    }

    inline int32_t Viewport::getActualHeight() const
    {
        return mActualHeight;
    }

    inline const Matrix4 &Viewport::getViewportMatrix() const
    {
        return mMatrix;
    }

    inline void Viewport::setBackgroundColor(const Color4 &rkColor)
    {
        mBkgndColor = rkColor;
    }

    inline const Color4 &Viewport::getBackgroundColor() const
    {
        return mBkgndColor;
    }
}
