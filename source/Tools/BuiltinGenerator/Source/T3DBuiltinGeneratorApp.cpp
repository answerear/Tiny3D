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

// For debug
// D:\private\Tiny3D\assets\editor\builtin -t


#include "T3DBuiltinGeneratorApp.h"
#include "T3DBuiltinGenerator.h"


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

            // 解析命令行参数
            GeneratorOptions opts;
            if (!parseCommandList(argc, argv, opts))
            {
                BGEN_LOG_ERROR("Parse command line failed !");
                break;
            }

            BuiltinGenerator *generator = T3D_NEW BuiltinGenerator();
            TResult rt = generator->run(opts.outputPath, opts.reservedTemp);
            if (T3D_FAILED(rt))
            {
                T3D_SAFE_DELETE(generator);
                break;
            }
            
            T3D_SAFE_DELETE(generator);
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

    bool BuiltinGeneratorApp::parseCommandList(int32_t argc, char *argv[], GeneratorOptions &options)
    {
        printCommand(argc, argv);

        bool ret = false;

        if (argc == 2)
        {
            const char *arg = argv[1];
            if (arg[0] == '-' && arg[1] == '?')
            {
                // 显示帮助
                printHelp();
                ret = true;
            }
            else
            {
                // 只有一个参数，就是输出路径
                options.outputPath = Dir::formatPath(argv[1]);
                ret = true;
            }
        }
        else if (argc == 3)
        {
            int32_t i = 1;

            while (i < argc)
            {
                const char *arg = argv[i];
                if (arg[0] == '-' && arg[1] == 't')
                {
                    // 保留临时文件夹
                    options.reservedTemp = true;
                    ret = true;
                }
                else
                {
                    options.outputPath = arg;
                    ret = true;
                }
                ++i;

                if (!ret)
                {
                    break;
                }
            }
        }
        else
        {
            printHelp();
        }

        return ret;
    }

    //--------------------------------------------------------------------------

    void BuiltinGeneratorApp::printCommand(int32_t argc, char* argv[]) const
    {
        int i = 0;

        for (i = 1; i < argc; ++i)
        {
            if (i > 1)
                printf(" ");
            printf(argv[i]);
        }

        printf("\n");
    }
    
    //--------------------------------------------------------------------------

    void BuiltinGeneratorApp::printHelp() const
    {
        printf("Usage: BuiltinGenerator <output path> <options>\n");
        printf("Arguments: \n");
        printf("\t-?         : Display the help information.\n");
        printf("\toutput path : The output path of builtin resource.\n");
        printf("\toptions :\n");
        printf("\t\t-t : Reserve the temporary folders for output.\n");
    }
    
    //--------------------------------------------------------------------------
}


int main(int argc, char *argv[])
{
    using namespace Tiny3D;

    Dir::setCachePathInfo("Tiny3D", "Tiny3D");
    
    Agent *theEngine = T3D_NEW Agent();
    Settings settings;
    settings.pluginSettings.pluginPath = ".";
    settings.pluginSettings.plugins.push_back("FileSystemArchiveEditor");
    settings.pluginSettings.plugins.push_back("FreeImageCodecEditor");
    settings.pluginSettings.plugins.push_back("NullRendererEditor");
    settings.renderSettings.renderer = "NullRenderer";
    TResult ret = theEngine->init(argc, argv, true, false, settings);
    if (T3D_SUCCEEDED(ret))
        theEngine->run();

    T3D_SAFE_DELETE(theEngine);

    return ret;
}

