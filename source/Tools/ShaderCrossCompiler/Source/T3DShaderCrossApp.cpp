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

#include "T3DShaderCrossApp.h"
#include "T3DSCCPrerequisites.h"
#include "T3DShaderCross.h"


// For debug
//// "$(SolutionDir)..\..\..\assets\scripts\BuiltinBox.material" -t hlsl -m 50
//// BuiltinBox.material BuiltinConstant.constant BuiltinProgram.program -t hlsl -m 50 -O3 -p $(SolutionDir)..\..\..\assets\scripts\
//// materials/BuiltinNoTexture.material program/BuiltinConstant.constant program/BuiltinProgram.program -t hlsl -m 50 -O3 -b -p $(SolutionDir)..\..\..\assets\builtin

// For debug
//// "$(SolutionDir)..\..\assets\builtin\materials\Test.Shader" -t hlsl
// "$(SolutionDir)..\..\assets\editor\builtin\shaders\Tiny3DStandard.shader" -t hlsl -o "$(SolutionDir)..\..\assets\editor\builtin\shaders"

// To hlsl for Samples
// Unlit vertex shader : "$(SolutionDir)..\..\assets\Samples\shaders\GeometryApp.vshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0
// Unlit pixel shader : "$(SolutionDir)..\..\assets\Samples\shaders\GeometryApp.pshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0
// Lit vertex shader : "$(SolutionDir)..\..\assets\Samples\shaders\LitGeometryApp.vshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0
// Lit pixel shader : "$(SolutionDir)..\..\assets\Samples\shaders\LitGeometryApp.pshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0
// Shadow pass vertex shader : "$(SolutionDir)..\..\assets\Samples\shaders\ShadowPass.vshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0
// Forward pass vertex shader : "$(SolutionDir)..\..\assets\Samples\shaders\ForwardPass.vshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0
// Forward pass pixel shader : "$(SolutionDir)..\..\assets\Samples\shaders\ForwardPass.pshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0
// Skin Shadow pass vertex shader : "$(SolutionDir)..\..\assets\Samples\shaders\SkinShadowPass.vshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0
// Skin Forward pass vertex shader : "$(SolutionDir)..\..\assets\Samples\shaders\SkinForwardPass.vshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0
// GPU Skin Shadow pass vertex shader : "$(SolutionDir)..\..\assets\Samples\shaders\GPUSkinShadowPass.vshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0
// GPU Skin Forward pass vertex shader : "$(SolutionDir)..\..\assets\Samples\shaders\GPUSkinForwardPass.vshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders\output" -N -O0


Tiny3D::ShaderCrossApp theApp;


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    TResult ShaderCrossApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
    {
        ShaderCrossPtr sc = ShaderCross::create();

        TResult ret = T3D_OK;
        
        do 
        {
            if (argc == 1)
            {
                T3D_LOG_ERROR(LOG_TAG, "Not enough parameters !");
                ret = T3D_ERR_INVALID_PARAM;
                break;
            }

            if (!sc->compile(argc-1, (const char**)(argv+1)))
            {
                T3D_LOG_ERROR(LOG_TAG, "Compile failed !");
                ret = T3D_ERR_FAIL;
                break;
            }
        } while (false);

        sc = nullptr;
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ShaderCrossApp::applicationDidEnterBackground()
    {
        
    }

    //--------------------------------------------------------------------------
    
    void ShaderCrossApp::applicationWillEnterForeground()
    {
        
    }

    //--------------------------------------------------------------------------
    
    void ShaderCrossApp::applicationWillTerminate()
    {
        
    }

    //--------------------------------------------------------------------------
    
    void ShaderCrossApp::applicationLowMemory()
    {
        
    }
    
    //--------------------------------------------------------------------------
}


int main(int argc, char *argv[])
{
    using namespace Tiny3D;

    Dir::setCachePathInfo("Tiny3D", "ShaderCrossCompiler");
    
    Agent *theEngine = T3D_NEW Agent();
    Settings settings;
    settings.pluginSettings.pluginPath = ".";
    settings.pluginSettings.plugins.push_back("FileSystemArchiveEditor");

    // Pre-scan argv to find -t parameter and select renderer accordingly
    String targetLang = "hlsl";
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-t") == 0 && i + 1 < argc)
        {
            targetLang = argv[i + 1];
            break;
        }
    }

    if (targetLang == "glsl" || targetLang == "essl")
    {
        settings.pluginSettings.plugins.push_back("GL4RendererConsole");
        settings.renderSettings.renderer = RHIRenderer::OPENGL4_CONSOLE;
    }
    else if (targetLang == "spirv")
    {
        settings.pluginSettings.plugins.push_back("VKRendererConsole");
        settings.renderSettings.renderer = RHIRenderer::VULKAN_CONSOLE;
    }
    else
    {
        settings.pluginSettings.plugins.push_back("D3D11RendererConsole");
        settings.renderSettings.renderer = RHIRenderer::DIRECT3D11_CONSOLE;
    }
    settings.logSettings.tag = LOG_TAG;
    TResult ret = theEngine->init(argc, argv, true, false, settings);
    if (ret == T3D_OK)
        theEngine->run();

    T3D_SAFE_DELETE(theEngine);

    return ret;
}

