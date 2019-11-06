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


#ifndef __T3D_SCRIPT_PARSER_MATERIAL_H__
#define __T3D_SCRIPT_PARSER_MATERIAL_H__


#include "Kernel/T3DScriptParserBase.h"


namespace Tiny3D
{
    class ScriptParserMaterial : public ScriptParserBase
    {
    public:
        /** 创建对象 */
        static ScriptParserMaterialPtr create();

        /** 析构函数 */
        virtual ~ScriptParserMaterial();

        /** 重写基类 ScriptParserBase::parseObject 接口 */
        virtual TResult parseObject(
            DataStream &stream, Object *object, uint32_t version) override;

    protected:
        /** 构造函数 */
        ScriptParserMaterial();

        /** 解析 Material 对象属性 */
        TResult parseProperties(
            DataStream &stream, Material *material, uint32_t version);

        /** 解析 Material 的 lod_values 属性*/
        TResult parseLODValus(
            DataStream &stream, Material *material, uint32_t version);

        /** 解析 Material 的 lod_strategy 属性 */
        TResult parseLODStrategy(
            DataStream &stream, Material *material, uint32_t version);

        /** 解析 Material 的 receive_shadows 属性 */
        TResult parseReceiveShadows(
            DataStream &stream, Material *material, uint32_t version);

        /** 解析 Material 的 transparency_casts_shadows 属性 */
        TResult parseTransparencyCastsShadows(
            DataStream &stream, Material *material, uint32_t version);

        /** 解析 Material 的 set_texture_alias 属性 */
        TResult parseSetTextureAlias(
            DataStream &stream, Material *material, uint32_t version);
    };
}


#endif  /*__T3D_SCRIPT_PARSER_MATERIAL_H__*/
