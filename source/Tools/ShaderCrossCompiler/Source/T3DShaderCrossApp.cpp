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

#include "T3DShaderCrossApp.h"

#include "T3DSCCPrerequisites.h"
#include "T3DShaderCross.h"


// For debug
// "$(SolutionDir)..\..\..\assets\scripts\BuiltinBox.material" -t hlsl -m 50
// BuiltinBox.material BuiltinConstant.constant BuiltinProgram.program -t hlsl -m 50 -O3 -p $(SolutionDir)..\..\..\assets\scripts\
// materials/BuiltinNoTexture.material program/BuiltinConstant.constant program/BuiltinProgram.program -t hlsl -m 50 -O3 -b -p $(SolutionDir)..\..\..\assets\builtin

// For debug
// "$(SolutionDir)..\..\assets\builtin\materials\Test.Shader" -t hlsl


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

    Agent *theEngine = new Agent();
    Settings settings;
    settings.pluginSettings.pluginPath = ".";
    settings.pluginSettings.plugins.push_back("FileSystemArchive");
    settings.pluginSettings.plugins.push_back("NullRenderer");
    settings.renderSettings.renderer = "NullRenderer";
    TResult ret = theEngine->init(argc, argv, true, settings);
    if (ret == T3D_OK)
        theEngine->run();

    delete theEngine;

    return ret;
}

