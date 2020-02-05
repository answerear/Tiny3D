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


#include "Kernel/T3DObject.h"
#include "Memory/T3DSmartPtr.h"
#include "Component/T3DComponentCreator.h"
#include "T3DErrorDef.h"
#include "Component/T3DTransform3D.h"
#include "Component/T3DCamera.h"
#include "Component/T3DCube.h"
#include "Component/T3DGlobe.h"

namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ComponentCreatorPtr ComponentCreator::create()
    {
        ComponentCreatorPtr creator = new ComponentCreator();
        creator->release();
        return creator;
    }

    //--------------------------------------------------------------------------

    ComponentCreator::ComponentCreator()
    {

    }

    //--------------------------------------------------------------------------

    ComponentCreator::~ComponentCreator()
    {
    }

    //--------------------------------------------------------------------------

    String ComponentCreator::getType() const
    {
        return "Component";
    }

    //--------------------------------------------------------------------------

    ComponentPtr ComponentCreator::createObject(int32_t argc, ...) const
    {
        ComponentPtr component;

        do 
        {
            va_list params;
            va_start(params, argc);
            va_list args = va_arg(params, va_list);

            String type = va_arg(args, const char *);

            if (type == "Transform3D")
            {
                component = Transform3D::create();
            }
            else if (type == "Camera")
            {
                component = Camera::create();
            }
            else if (type == "Cube")
            {
                Vector3 *center = va_arg(args, Vector3 *);
                Vector3 *extent = va_arg(args, Vector3 *);
                component = Cube::create(*center, *extent);
            }
            else if (type == "Globe")
            {
                Vector3 *center = va_arg(args, Vector3 *);
                Real *radius = va_arg(args, Real *);
                component = Globe::create(*center, *radius);
            }

            va_end(params);

        } while (0);
        return component;
    }
}
