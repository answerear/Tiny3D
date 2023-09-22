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


#ifndef __T3D_SCC_SHADER_CROSS_NEW_H__
#define __T3D_SCC_SHADER_CROSS_NEW_H__


#include "T3DSCCPrerequisites.h"
#include "SLParserTypes.h"


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

        bool compile(int32_t argc, const char *argv[]);

        bool compile(const String &input, const Args &args);

        bool compile(const String &input, const String &output, const Args &args);

    protected:
        void printUsage();

        void printVersion();

        bool parse(int32_t argc, const char* argv[], Args& args);

        bool readSourceFile(String& content, const String& input);

        bool compile(shaderlab::SLShader *source);

        bool translate(const shaderlab::SLShader& src, ShaderPtr dst);

        bool translate(const shaderlab::SLPropValue& src, ShaderPtr dst, uint32_t &offset);

        bool translate(const shaderlab::SLSubShader& src, ShaderPtr dst);

        bool translate(const shaderlab::SLPassBase& src, TechniquePtr tech);

        bool translate(const shaderlab::SLNormalPass& src, TechniquePtr tech);

        bool translate(const shaderlab::SLShaderState& src, RenderState &state);

        bool translate(const shaderlab::SLStencilOperation& src, DepthStencilState::StencilOpDesc &dst);

        bool translate(const shaderlab::SLProgram& src, PassPtr pass);
        
        BlendFactor getBlendFactor(shaderlab::SLFloat src) const;

        BlendOperation getBlendOp(shaderlab::SLFloat src) const;

        CullingMode getCulling(shaderlab::SLFloat src) const;

        CompareFunction getCompare(shaderlab::SLFloat src) const;
        
        StencilOp getStencilOp(shaderlab::SLFloat src) const;
    private:
        String  mInputFile {};
        String  mOutputDir {};
        Args    mArgs {};
    };
}


#endif  /*__T3D_SCC_SHADER_CROSS_NEW_H__*/

