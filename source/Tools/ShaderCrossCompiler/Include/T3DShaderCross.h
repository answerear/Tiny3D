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


#ifndef __T3D_SCC_SHADER_CROSS_H__
#define __T3D_SCC_SHADER_CROSS_H__


#include "T3DSCCPrerequisites.h"
#include "SLParserTypes.h"
#include "ShaderScriptObject.pb.h"


namespace shaderlab
{
    struct SLShader;
    struct SLPropValue;
    struct SLSubShader;
    struct SLPassBase;
    struct SLNormalPass;
    struct SLUsePass;
    struct SLShaderState;
    struct SLStencilOperation;
    struct SLProgram;
    struct SLFloat;
}

namespace Tiny3D
{
    class ShaderCross;

    T3D_DECLARE_SMART_PTR(ShaderCross);

    class ShaderCross : public Object
    {
    public:
        static ShaderCrossPtr create();

        virtual ~ShaderCross();

        bool compile(int32_t argc, const char *argv[]);

        bool compile(const String &input, const Args &args);

        bool compile(const String &input, const String &output, const Args &args);

    protected:
        ShaderCross();

        void printUsage();

        void printVersion();

        bool parse(int32_t argc, const char* argv[], Args& args);

        bool readSourceFile(String& content, const String& input);

        bool compile(shaderlab::SLShader *source);

        bool translate(const shaderlab::SLShader& src, Script::ShaderSystem::Shader* dst);

        bool translate(const shaderlab::SLPropValue& src, Script::ShaderSystem::Property* dst);

        bool translate(const shaderlab::SLSubShader& src, Script::ShaderSystem::SubShader* dst);

        bool translate(const shaderlab::SLPassBase& src, Script::ShaderSystem::Pass* dst);

        bool translate(const shaderlab::SLUsePass& src, Script::ShaderSystem::Pass* dst);

        bool translate(const shaderlab::SLNormalPass& src, Script::ShaderSystem::Pass* dst);

        bool translate(const shaderlab::SLShaderState& src, Script::ShaderSystem::State* dst);

        bool translate(const shaderlab::SLStencilOperation& src, Script::ShaderSystem::StencilOp* dst);

        bool translate(const shaderlab::SLProgram& src, Script::ShaderSystem::Program* dst);

        Script::ShaderSystem::BlendFactor getBlendFactor(shaderlab::SLFloat src) const;

        Script::ShaderSystem::BlendOp getBlendOp(shaderlab::SLFloat src) const;

        Script::ShaderSystem::Cull getCulling(shaderlab::SLFloat src) const;

        Script::ShaderSystem::Comp getCompare(shaderlab::SLFloat src) const;

        Script::ShaderSystem::StencilOp_Op getStencilOp(shaderlab::SLFloat src) const;

        Script::ShaderSystem::ProgramType getProgramType(shaderlab::ProgramType src) const;

    private:
        String  mInputFile;
        String  mOutputDir;
        Args    mArgs;
    };
}


#endif  /*__T3D_SCC_SHADER_CROSS_H__*/

