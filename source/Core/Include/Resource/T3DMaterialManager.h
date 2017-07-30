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

#ifndef __T3D_MATERIAL_MANAGER_H__
#define __T3D_MATERIAL_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    class T3D_ENGINE_API MaterialManager 
        : public Singleton<MaterialManager>
        , public ResourceManager
    {
    public:
        MaterialManager();
        virtual ~MaterialManager();

        virtual MaterialPtr loadMaterial(const String &name, Material::MaterialType matType);

    protected:
        virtual ResourcePtr create(const String &name, int32_t argc, va_list args) override;
    };

    #define T3D_MATERIAL_MGR        (MaterialManager::getInstance())
}

#endif  /*__T3D_MATERIAL_MANAGER_H__*/
