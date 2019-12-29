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


#ifndef __T3D_OBJECT_H__
#define __T3D_OBJECT_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * @brief 引擎所有对象基类
     */
    class T3D_ENGINE_API Object
    {
    public:
        /**
         * @brief 构造函数
         */
        Object();

        /**
         * @brief 析构函数
         */
        virtual ~Object();

        /**
         * @brief 持有对象，让对象引用计数加一
         */
        Object *acquire();

        /**
         * @brief 释放对象，让对象引用计数减一
         */
        void release();

        /**
         * @brief 返回对象当前引用计数
         */
        uint32_t referCount() const
        {
            return mReferCount;
        }

    private:
        uint32_t    mReferCount;
    };
}


#endif  /*__T3D_OBJECT_H__*/
