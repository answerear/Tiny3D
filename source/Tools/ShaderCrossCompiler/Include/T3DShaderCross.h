/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/


#ifndef __T3D_SCC_SHADER_CROSS_H__
#define __T3D_SCC_SHADER_CROSS_H__


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

        void printCommand(int32_t argc, const char *argv[]) const;

        bool parse(int32_t argc, const char* argv[], Args& args);

        bool readSourceFile(String& content, const String& input);

        bool compile(shaderlab::SLShader *source);

        bool translate(const shaderlab::SLShader& src, ShaderPtr dst);

        bool translate(const shaderlab::SLPropValue& src, ShaderPtr dst);

        bool translate(const shaderlab::SLSubShader& src, ShaderPtr dst);

        bool translate(const shaderlab::SLPassBase& src, TechniquePtr tech);

        bool translate(const shaderlab::SLNormalPass& src, TechniquePtr tech);

        bool translate(const shaderlab::SLShaderState& src, RenderStatePtr state);

        bool translate(const shaderlab::SLStencilOperation& src, StencilOpDesc &dst);

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


#endif  /*__T3D_SCC_SHADER_CROSS_H__*/

