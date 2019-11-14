﻿/*******************************************************************************
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
    /** 動態庫資源管理器 */
    class T3D_ENGINE_API DylibManager 
        : public Singleton<DylibManager>
        , public ResourceManager
    {
    public:
        /** 創建動態庫管理器對象 */
        static DylibManagerPtr create();

        /** 析構函數 */
        virtual ~DylibManager();

        /** 加載動態庫 */
        virtual DylibPtr loadDylib(const String &name);

        /** 卸載動態庫 */
        virtual TResult unloadDylib(DylibPtr dylib);

    protected:
        /** 構造函數 */
        DylibManager();

        /** @brief 重寫基類 Resource::create() 接口 */
        virtual ResourcePtr create(
            const String &strName, int32_t argc, va_list args) override;
    };

    #define T3D_DYLIB_MGR   (DylibManager::getInstance())
}


#endif  /*__T3D_DYLIB_MANAGER_H__*/
