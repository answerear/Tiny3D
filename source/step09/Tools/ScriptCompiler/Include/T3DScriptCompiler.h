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

#ifndef __T3D_SCRIPT_COMPILER_H__
#define __T3D_SCRIPT_COMPILER_H__


#include "T3DScriptPrerequisites.h"
#include "T3DScriptASTree.h"


namespace Tiny3D
{
    class ScriptLexer;
    class ScriptParser;

    class ScriptCompiler
    {
        friend class AbstractTreeBuilder;

    public:
        typedef std::unordered_map<String, uint32_t> IdMap;

        struct Options
        {
            Options()
            {

            }
        };

    public:
        /**
         * @brief 构造函数
         */
        ScriptCompiler();

        /**
         * @brief 析构函数
         */
        virtual ~ScriptCompiler();

        /**
         * @brief 输出使用方法
         */
        void usage();

        /**
         * @brief 编译源码，生成二进制码流
         * @param [in] input : 要编译的源码文件
         * @param [in] output : 编译后生成的文件全路径
         * @param [in] opt : 编译选项
         * @return 编译成功返回true
         */
        bool compile(const String &input, const String &output, Options opt = {});

    public:

    private:
        bool isNameExcluded(const ObjectAbstractNode& node, AbstractNode *parent);
        void initWordMap();

    private:
        IdMap mIds;

        typedef std::map<String, String> Environment;
        Environment mEnv;

        ScriptLexer     *mLexer;
        ScriptParser    *mParser;
    };
}


#endif  /*__T3D_SCRIPT_COMPILER_H__*/
