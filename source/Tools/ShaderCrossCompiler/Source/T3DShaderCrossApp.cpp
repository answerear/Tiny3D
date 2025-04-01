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
// "$(SolutionDir)..\..\assets\editor\builtin\shaders\Default-Material.shader" -t hlsl -o "$(SolutionDir)..\..\assets\editor\builtin\materials"

// To hlsl for Samples
// vertex shader : "$(SolutionDir)..\..\assets\Samples\shaders\GeometryApp.vshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders" -N
// pixel shader : "$(SolutionDir)..\..\assets\Samples\shaders\GeometryApp.pshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders" -N
// vertex shader : "$(SolutionDir)..\..\assets\Samples\shaders\LitGeometryApp.vshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders" -N
// pixel shader : "$(SolutionDir)..\..\assets\Samples\shaders\LitGeometryApp.pshader" -t hlsl -o "$(SolutionDir)..\..\assets\Samples\shaders" -N



Tiny3D::ShaderCrossApp theApp;


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    bool ShaderCrossApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
    {
        ShaderCrossPtr sc = ShaderCross::create();

        bool ret = false;
        
        do 
        {
            if (argc == 1)
            {
                T3D_LOG_ERROR(LOG_TAG, "Not enough parameters !");
                ret = false;
                break;
            }

            if (!sc->compile(argc-1, (const char**)(argv+1)))
            {
                ret = false;
                break;
            }

            ret = true;
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

    Dir::setCachePathInfo("Tiny3D", "Tiny3D");
    
    Agent *theEngine = new Agent();
    Settings settings;
    settings.pluginSettings.pluginPath = ".";
    settings.pluginSettings.plugins.push_back("FileSystemArchiveEditor");
    settings.pluginSettings.plugins.push_back("NullD3D11Renderer");
    settings.renderSettings.renderer = RHIRenderer::NULL_DIRECT3D11;
    settings.logSettings.tag = LOG_TAG;
    TResult ret = theEngine->init(argc, argv, true, false, settings);
    if (ret == T3D_OK)
        theEngine->run();

    delete theEngine;

    return ret;
}

