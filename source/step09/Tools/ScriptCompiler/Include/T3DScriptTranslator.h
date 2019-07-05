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

#ifndef __T3D_SCRIPT_TRANSLATOR_H__
#define __T3D_SCRIPT_TRANSLATOR_H__


#include "T3DScriptPrerequisites.h"
#include "T3DScriptType.h"
#include "T3DScriptASTree.h"


namespace Tiny3D
{
    class ScriptCompiler;

    //--------------------------------------------------------------------------

    class ScriptTranslator
    {
    public:
        T3D_DECLARE_INTERFACE(ScriptTranslator);

        /**
         * @brief 转换成对应格式，写到数据流对象中
         */
        virtual size_t translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node) = 0;

    protected:
        size_t processNode(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node);
    };

    //--------------------------------------------------------------------------

    class MaterialTranslator : public ScriptTranslator
    {
    public:
        virtual size_t translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node) override;
    };
}


#endif  /*__T3D_SCRIPT_TRANSLATOR_H__*/
