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


#ifndef __T3D_CREATOR_H__
#define __T3D_CREATOR_H__


#include "T3DPrerequisites.h"
#include "Kernel/T3DSmartPtr.h"


namespace Tiny3D
{
    template <typename T>
    class Creator
    {
    public:
        /**
         * @brief 获取字符串表示的对象类型
         */
        virtual String getType() const = 0;

        /**
         * @brief 创建具体对象
         */
        virtual SmartPtr<T> createObject(int32_t argc, ...) const = 0;
    };
}


#endif  /*__T3D_CREATOR_H__*/
