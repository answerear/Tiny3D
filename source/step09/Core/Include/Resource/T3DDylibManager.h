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


#ifndef __T3D_DYLIB_MANAGER_H__
#define __T3D_DYLIB_MANAGER_H__


#include "T3DPrerequisites.h"
#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    class T3D_ENGINE_API DylibManager 
        : public Singleton<DylibManager>
        , public ResourceManager
    {
    public:
        /**
         * @brief 创建动态库管理器对象
         */
        static DylibManagerPtr create();

        virtual ~DylibManager();

        virtual DylibPtr loadDylib(const String &name);

        virtual void unloadDylib(DylibPtr &dylib);

    protected:
        /**
         */
        DylibManager();

        /**
        * @brief 创建具体资源对象，具体子类实现该方法
        * @param [in] strName : 资源名称
        * @param [in] argc : 资源创建带的参数列表大小
        * @param [in] args : 参数列表
        */
        virtual ResourcePtr create(const String &strName, 
            int32_t argc, va_list args) override;
    };
}


#endif  /*__T3D_DYLIB_MANAGER_H__*/
