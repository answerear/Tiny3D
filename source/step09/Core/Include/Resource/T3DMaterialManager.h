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


#ifndef __T3D_MATERIAL_MANAGER_H__
#define __T3D_MATERIAL_MANAGER_H__


#include "Resource/T3DResourceManager.h"
#include "Resource/T3DMaterial.h"


namespace Tiny3D
{
    /**
     * @brief 材质资源管理器
     */
    class T3D_ENGINE_API MaterialManager
        : public Singleton<MaterialManager>
        , public ResourceManager
    {
    public:
        /**
         * @brief 创建材质管理器对象
         * @return 返回一个材质管理器对象
         */
        static MaterialManagerPtr create();

        /**
         * @brief 析构函数
         */
        virtual ~MaterialManager();

        /**
         * @brief 加载材质
         * @param [in] name : 材质资源名称
         * @param [in] matType : 材质类型
         * @return 返回一个新建材质对象
         * @see enum Material::MaterialType
         */
        virtual MaterialPtr loadMaterial(const String &name, 
            Material::MaterialType matType);

        /**
         * @brief 卸载材质资源
         * @param [in] material : 要卸载的材质资源对象
         * @return void
         */
        virtual void unloadMaterial(MaterialPtr &material);

    protected:
        /**
         * @brief 构造函数
         */
        MaterialManager();

        /**
         * @brief 重写基类接口，实现创建材质对象
         * @see ResourcePtr Resource::create(const String &name, int32_t argc,
         *      va_list args)
         */
        virtual ResourcePtr create(const String &name, int32_t argc, 
            va_list args) override;
    };

    #define T3D_MATERIAL_MGR        (MaterialManager::getInstance())
}


#endif  /*__T3D_MATERIAL_MANAGER_H__*/
