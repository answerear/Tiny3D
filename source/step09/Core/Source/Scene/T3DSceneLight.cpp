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


#include "Scene/T3DSceneLight.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    SceneLightPtr SceneLight::create(ID uID /* = E_CID_AUTOMATIC */)
    {
        SceneLightPtr light = new SceneLight(uID);
        light->release();
        return light;
    }

    //--------------------------------------------------------------------------

    SceneLight::SceneLight(ID uID /* = E_CID_AUTOMATIC */)
        : SceneTransform3D(uID)
    {

    }

    //--------------------------------------------------------------------------

    SceneLight::~SceneLight()
    {

    }

    //--------------------------------------------------------------------------

    const String &SceneLight::getType() const
    {
        static const String name = "SceneLight";
        return name;
    }

    //--------------------------------------------------------------------------

    ComponentPtr SceneLight::clone() const
    {
        SceneLightPtr newObj = SceneLight::create();
        TResult ret = cloneProperties(newObj);

        if (ret != T3D_OK)
        {
            newObj = nullptr;
        }

        return newObj;
    }

    //--------------------------------------------------------------------------

    TResult SceneLight::cloneProperties(ComponentPtr newObj) const
    {
        TResult ret = SceneTransform3D::cloneProperties(newObj);

        if (ret == T3D_OK)
        {

        }

        return ret;
    }
}
