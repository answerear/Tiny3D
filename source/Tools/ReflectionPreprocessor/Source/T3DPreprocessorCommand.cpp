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


#include "T3DPreprocessorCommand.h"


namespace Tiny3D
{
    //-------------------------------------------------------------------------
    
    PreprocessorCommand::PreprocessorCommand()
    {
        
    }

    //-------------------------------------------------------------------------
    
    PreprocessorCommand::~PreprocessorCommand()
    {
        
    }

    //-------------------------------------------------------------------------

    bool PreprocessorCommand::parse4Debug(int32_t argc, char* argv[], String& srcPath, String& dstPath, ClangArgs& args)
    {
        printCommand(argc, argv);

        bool ret = false;

        srcPath = argv[1];
        dstPath  = argv[2];

        args.clear();
        
        for (int32_t i = 3; i < argc; ++i)
        {
            const char *arg = argv[i];
            args.push_back(arg);
        }
        
        return ret;
    }

    //-------------------------------------------------------------------------

    bool PreprocessorCommand::parse(int32_t argc, char* argv[], PreprocessorOptions& options)
    {        
        printCommand(argc, argv);

        bool ret = false;
        
        if (argc == 2)
        {
            const char* arg = argv[1];
            if (arg[0] == '-' && arg[1] == '?')
            {
                // 显示帮助
                printHelp();
                ret = true;
            }
        }
        else if (argc == 4)
        {
            // ReflectionSettings.json
            options.SettingsPath = Dir::formatPath(argv[1]);
            // 源码根目录
            options.SourcePath = Dir::formatPath(argv[2]);

            const char *arg = argv[3];
            if (arg[0] == '-' && arg[1] == 'r')
            {
                options.IsRebuild = true;
            }
            else if (arg[0] == '-' && arg[1] == 'b')
            {
                options.IsRebuild = false;
            }
            
            ret = true;
        }
        else
        {
            // There were some mistakes.
            printHelp();
        }

        return ret;
    }

    //-------------------------------------------------------------------------

    void PreprocessorCommand::printCommand(int32_t argc, char* argv[]) const
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

    //-------------------------------------------------------------------------

    void PreprocessorCommand::printHelp() const
    {
        printf("Usage: rp <reflection settings directory> <source directory> [<options>]\n");
        printf("Arguments: \n");
        printf("\t-?                            : Display the help information.\n");
        printf("\treflection settings directory : The directory of 'ReflectionSettings.json' file.\n");
        printf("\tsource directory              : The root directory of source code.\n");
        printf("\t<options>                     :\n");
        printf("\t\t-r : Re-generate all reflection source files.");
        printf("\t\t-b : Check last generated time before generate reflection source files.");
    }

    //-------------------------------------------------------------------------
}

