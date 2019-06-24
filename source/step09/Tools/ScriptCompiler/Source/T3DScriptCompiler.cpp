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


#include "T3DScriptCompiler.h"
#include "T3DScriptLexer.h"
#include "T3DScriptParser.h"
#include "T3DScriptError.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    ScriptCompiler::ScriptCompiler()
        : mLexer(nullptr)
        , mParser(nullptr)
    {
        mLexer = new ScriptLexer();
        mParser = new ScriptParser(this);
    }

    //--------------------------------------------------------------------------

    ScriptCompiler::~ScriptCompiler()
    {
        delete mLexer;
        mLexer = nullptr;

        delete mParser;
        mParser = nullptr;
    }

    //--------------------------------------------------------------------------

    void ScriptCompiler::usage()
    {

    }

    //--------------------------------------------------------------------------

    bool ScriptCompiler::compile(const String &input, const String &output, 
        Options opt)
    {
        if (!Dir::exists(input))
        {
            T3D_LOG_ERROR("Compiler", "%s does not exist !", input.c_str());
            return false;
        }

        // 打开文件
        FileDataStream fs;

        if (!fs.open(input.c_str(), FileDataStream::E_MODE_READ_ONLY))
        {
            T3D_LOG_ERROR("Compiler", "Open %s failed !", input.c_str());
            return false;
        }

        // 读取文件
        size_t size = (size_t)fs.size();
        uint8_t *data = new uint8_t[size];
        size_t bytesOfRead = fs.read(data, size);
        if (size != bytesOfRead)
        {
            T3D_LOG_ERROR("Compiler", "Read %s data failed !", input.c_str());
            return false;
        }

        String str(data, data + size);

        delete []data;

        TokenListPtr tokens = ScriptLexer::getInstance().tokenize(str, input);
        AbstractNodeListPtr ast(new AbstractNodeList());
        bool ret = ScriptParser::getInstance().parse(tokens, ast);

        return ret;
    }

    //--------------------------------------------------------------------------

    bool ScriptCompiler::isNameExcluded(const ObjectAbstractNode& node, AbstractNode* parent)
    {
        // Run past the listener
        bool excludeName = false;

        // Process the built-in name exclusions
        if (node.id == ID_EMITTER || node.id == ID_AFFECTOR)
        {
            // emitters or affectors inside a particle_system are excluded
            while (parent && parent->type == ANT_OBJECT)
            {
                ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(parent);
                if (obj->id == ID_PARTICLE_SYSTEM)
                    return true;
                parent = obj->parent;
            }
        }
        else if (node.id == ID_PASS)
        {
            // passes inside compositors are excluded
            while (parent && parent->type == ANT_OBJECT)
            {
                ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(parent);
                if (obj->id == ID_COMPOSITOR)
                    return true;
                parent = obj->parent;
            }
        }
        else if (node.id == ID_TEXTURE_SOURCE)
        {
            // Parent must be texture_unit
            while (parent && parent->type == ANT_OBJECT)
            {
                ObjectAbstractNode *obj = static_cast<ObjectAbstractNode*>(parent);
                if (obj->id == ID_TEXTURE_UNIT)
                    return true;
                parent = obj->parent;
            }
        }

        return false;
    }
}

