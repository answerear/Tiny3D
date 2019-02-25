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


#ifndef __T3D_MATERIAL_H__
#define __T3D_MATERIAL_H__


#include "T3DResource.h"


namespace Tiny3D
{
    /**
     * @brief 材质资源
     */
    class T3D_ENGINE_API Material : public Resource
    {
    public:
        /** 创建 Material 对象 */
        static MaterialPtr create(const String &name);

        /** 析构函数 */
        virtual ~Material();

        /** 重写 Resource::getType() */
        virtual Type getType() const override;

    protected:
        /** 构造函数 */
        Material(const String &name);

        /** 重写 Resource::load() */
        virtual TResult load() override;

        /** 重写 Resource::unlaod() */
        virtual TResult unload() override;

        /** 重写 Resource::clone() */
        virtual ResourcePtr clone() const override;

    protected:
    };
}


#endif  /*__T3D_MATERIAL_H__*/
