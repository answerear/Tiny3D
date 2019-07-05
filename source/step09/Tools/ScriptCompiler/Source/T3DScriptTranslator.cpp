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


#include "T3DScriptTranslator.h"
#include "T3DScriptError.h"
#include "T3DScriptCompiler.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    size_t ScriptTranslator::processNode(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node)
    {
        if (node->type != ANT_OBJECT)
            return 0;

        // Abstract objects are completely skipped
        ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(node.get());
        if (obj->abstrct)
            return 0;

        // Retrieve the translator to use
        ScriptTranslator *translator = compiler->getTranslator(node);

        size_t ret = 0;

        if (translator)
        {
            ret = translator->translate(compiler, stream, node);
        }
        else
        {
            ScriptError::printError(CERR_UNEXPECTEDTOKEN, obj->name, obj->file, obj->line);
        }

        return ret;
    }

    size_t ScriptTranslator::translate(ScriptCompiler *compiler, DataStream &stream, const AbstractNodePtr &node)
    {
        ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(node.get());

        if (obj->name.empty())
        {
            ScriptError::printError(CERR_OBJECTNAMEEXPECTED, "No name object", obj->file, obj->line);
            return 0;
        }

        size_t bytesOfWritten = 0;

        // 类型
        stream.write(&obj->type, sizeof(obj->type));
        // ID
        stream.write(&obj->id, sizeof(obj->id));
        // 名称长度
        uint16_t len = obj->name.length();
        stream.write(&len, sizeof(len));
        // 名称字符串
        stream.write((void*)obj->name.c_str(), len);
        bytesOfWritten += len;

        // 属性
        for (auto i = obj->children.begin(); i != obj->children.end(); ++i)
        {
            if ((*i)->type == ANT_PROPERTY)
            {
                PropertyAbstractNode *prop = static_cast<PropertyAbstractNode*>((*i).get());
                switch (prop->id)
                {
                case ID_LOD_VALUES:
                    {
                        // LOD数量
                        len = prop->values.size();
                        stream.write(&len, sizeof(len));
                        // LOD值
                        for (auto j = prop->values.begin(); j != prop->values.end(); ++j)
                        {
                            float32_t val;

                        }
                    }
                    break;
                }
            }
        }

        return bytesOfWritten;
    }

    //--------------------------------------------------------------------------
}

