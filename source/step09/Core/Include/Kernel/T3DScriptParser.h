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


#ifndef __T3D_SCRIPT_PARSER_H__
#define __T3D_SCRIPT_PARSER_H__


#include "Kernel/T3DScriptParserBase.h"


namespace Tiny3D
{
    class T3D_ENGINE_API ScriptParser 
        : public Singleton<ScriptParser>
        , public ScriptParserBase
    {
    protected:
        struct TSCFileHeader
        {
            char        magic[4];   /**< 文件魔数，用于标识文件 */
            uint32_t    version;    /**< 文件版本号 */
            uint32_t    filesize;   /**< 文件大小 */
        };

    public:
        /** 创建对象 */
        static ScriptParserPtr create();

        /** 析构函数 */
        virtual ~ScriptParser();

        /** 解析数据流到material对象中 */
        TResult parse(DataStream &stream, Object *material);

        /** 获取对应操作码的对象脚本解析器 */
        ScriptParserBase *getObjectParser(uint16_t opcode);

    protected:
        /** 构造函数 */
        ScriptParser();

        /** 重写基类 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        ScriptParserMaterialPtr         mMaterialParser;
        ScriptParserTechniquePtr        mTechParser;
        ScriptParserPassPtr             mPassParser;
        ScriptParserTextureUnitPtr      mTexUnitParser;
        ScriptParserSamplerPtr          mSamplerParser;
        ScriptParserGPUProgramPtr       mGPUParser;
        ScriptParserGPUProgramRefPtr    mGPURefParser;
        ScriptParserGPUProgramExPtr     mGPUParserEx;
        ScriptParserGPUProgramRefExPtr  mGPURefParserEx;
    };
}


#endif  /*__T3D_SCRIPT_PARSER_H__*/
