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

#include "T3DBuiltinGeneratorApp.h"



Tiny3D::BuiltinGeneratorApp theApp;


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    bool BuiltinGeneratorApp::applicationDidFinishLaunching(int32_t argc, char *argv[])
    {
        bool ret = false;
        
        do 
        {
            if (argc == 1)
            {
                BGEN_LOG_ERROR("Not enough parameters !");
                ret = false;
                break;
            }
            ret = true;
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void BuiltinGeneratorApp::applicationDidEnterBackground()
    {
        
    }

    //--------------------------------------------------------------------------
    
    void BuiltinGeneratorApp::applicationWillEnterForeground()
    {
        
    }

    //--------------------------------------------------------------------------
    
    void BuiltinGeneratorApp::applicationWillTerminate()
    {
        
    }

    //--------------------------------------------------------------------------
    
    void BuiltinGeneratorApp::applicationLowMemory()
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
    settings.pluginSettings.plugins.push_back("FileSystemArchive");
    settings.pluginSettings.plugins.push_back("NullRenderer");
    settings.renderSettings.renderer = "NullRenderer";
    TResult ret = theEngine->init(argc, argv, true, false, settings);
    if (ret == T3D_OK)
        theEngine->run();

    delete theEngine;

    return ret;
}

