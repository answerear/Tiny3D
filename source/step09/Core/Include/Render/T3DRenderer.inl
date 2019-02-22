/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
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
    inline const String &Renderer::getName() const
    {
        return mName;
    }

    inline TResult Renderer::setViewTransform(const Matrix4 &mat)
    {
        return setTransform(E_TS_VIEW, mat);
    }

    inline TResult Renderer::setWorldTransform(const Matrix4 &mat)
    {
        return setTransform(E_TS_WORLD, mat);
    }

    inline TResult Renderer::setProjectionTransform(const Matrix4 &mat)
    {
        return setTransform(E_TS_PROJECTION, mat);
    }


    inline ViewportPtr Renderer::getViewport() const
    {
        return mViewport;
    }

    inline Renderer::CullingMode Renderer::getCullingMode() const
    {
        return mCullingMode;
    }

    inline Renderer::RenderMode Renderer::getRenderMode() const
    {
        return mRenderMode;
    }
}

