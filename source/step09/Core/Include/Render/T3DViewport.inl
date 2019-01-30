﻿/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Agent)
 * Copyright (C) 2015-2019  Answer Wong
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
    inline RenderTargetPtr Viewport::getRenderTarget() const
    {
        return mRenderTarget;
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

    inline size_t Viewport::getActualLeft() const
    {
        return mActualLeft;
    }

    inline size_t Viewport::getActualTop() const
    {
        return mActualTop;
    }

    inline size_t Viewport::getActualWidth() const
    {
        return mActualWidth;
    }

    inline size_t Viewport::getActualHeight() const
    {
        return mActualHeight;
    }

    inline const Matrix4 &Viewport::getViewportMatrix() const
    {
        return mMatrix;
    }

    inline void Viewport::setBkgndColor(const Color4 &color)
    {
        mBkgndColor = color;
    }

    inline const Color4 &Viewport::getBkgndColor() const
    {
        return mBkgndColor;
    }
}
