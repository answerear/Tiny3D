/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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


#ifndef __T3D_MATERIAL_H__
#define __T3D_MATERIAL_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    /**
     * @brief 材质资源
     */
    class T3D_ENGINE_API Material : public Resource
    {
    public:
        /**
         * @brief 材质类型
         */
        enum MaterialType
        {
            E_MT_DEFAULT = 0,   /**< 默认是从资源加载的材质 */
            E_MT_MANUAL,        /**< 有调用者创建的材质 */
        };

        /** 创建 Material 对象 */
        static MaterialPtr create(const String &name, MaterialType type);

        /** 析构函数 */
        virtual ~Material();

        /** 重写 Resource::getType() */
        virtual Type getType() const override;

    protected:
        /** 构造函数 */
        Material(const String &name);

        /**
         * @brief 初始化对象
         */
        virtual TResult init();

        /** 重写 Resource::load() */
        virtual TResult load() override;

        /** 重写 Resource::unlaod() */
        virtual TResult unload() override;

        /** 重写 Resource::clone() */
        virtual ResourcePtr clone() const override;

    protected:
        MaterialType        mMaterialType;      /**< 材质资源类型 */
    };
}


#endif  /*__T3D_MATERIAL_H__*/
