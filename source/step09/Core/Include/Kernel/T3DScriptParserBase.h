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


#ifndef __T3D_SCRIPT_PARSER_BASE_H__
#define __T3D_SCRIPT_PARSER_BASE_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"
#include "Kernel/T3DObject.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ScriptParserBase : public Object
    {
    public:
        /** 析构函数 */
        virtual ~ScriptParserBase();

        /** 解析数据流到object对象中 */
        virtual TResult parse(DataStream &stream, Object *object) = 0;

    protected:
        /** 构造函数 */
        ScriptParserBase();

        /** 读取一个字符串 */
        TResult parseString(DataStream &stream, String &str);

        char    *mText;     /**< 用于存放读取字符串，避免重复申请内存，只在不够的时候动态扩展大小 */
        size_t  mTextLen;   /**< 用户存放读取字符串的缓冲区大小 */
    };
}


#endif  /*__T3D_SCRIPT_PARSER_BASE_H__*/
