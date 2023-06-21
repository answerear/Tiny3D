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
        printf("Usage: rp <reflection settings directory> <source directory> <options>]\n");
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

