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


#include "T3DConverterCommand.h"
#include "T3DConverterOptions.h"


namespace Tiny3D
{
    //--------------------------------------------------------------------------

    bool ConverterCommand::parse(int32_t argc, char *argv[], ConverterOptions &opts)
    {
        bool ret = true;

        do
        {
            printCommand(argc, argv);

            if (argc < 3)
            {
                printf("Invalid arguments.\n");
                printHelp();
                ret = false;
                break;
            }

            bool bShowHelp = false;

            for (int32_t i = 1; i < argc; ++i)
            {
                const char *arg = argv[i];
                size_t len = strlen(arg);
                if (len > 1 && arg[0] == '-')
                {
                    if (arg[1] == '?')
                    {
                        bShowHelp = true;
                    }
                    else if (arg[1] == 'v')
                    {
                        opts.verbose = true;
                    }
                    else if (arg[1] == 't')
                    {
                        opts.isTxt = true;
                    }
                    else if (arg[1] == 'i')
                    {
                        opts.srcFileType = parseFileType(argv[++i]);
                    }
                    else if (arg[1] == 'o')
                    {
                        opts.dstFileType = parseFileType(argv[++i]);
                    }
                    else if (arg[1] == 'b')
                    {
                        opts.boundType = parseBoundType(argv[++i]);
                    }
                    else if (arg[1] == 'm')
                    {
                        opts.extraPath = argv[++i];
                    }
                    else if (arg[1] == 'd')
                    {
                        opts.defaultMaterialPath = argv[++i];
                    }
                    else if (arg[1] == 'r')
                    {
                        opts.defaultResourcePath = argv[++i];
                    }
                    else if (arg[1] == 's')
                    {
                        opts.compiledShaderPath = argv[++i];
                    }
                    else if (arg[1] == 'a')
                    {
                        opts.isGeneratingMeta = true;
                    }
                }
                else if (opts.srcPath.empty())
                {
                    opts.srcPath = arg;
                }
                else if (opts.dstDir.empty())
                {
                    opts.dstDir = arg;
                }
            }

            if (bShowHelp || opts.srcPath.empty() || opts.dstFileType == MeshFileType::kAuto)
            {
                // 显示帮助信息
                // 输入文件全路径是必须指定的
                // 输出文件格式是必须指定的
                if (opts.srcPath.empty())
                {
                    MCONV_LOG_ERROR("Input file full path is empty.")
                }
                if (opts.dstFileType == MeshFileType::kAuto)
                {
                    MCONV_LOG_ERROR("Output file type is empty.")
                }
                
                printHelp();
                ret = false;
                break;
            }

            // 处理一些默认值
            if (opts.srcFileType == MeshFileType::kAuto || opts.dstDir.empty())
            {
                String dir, title, ext;
                Dir::parsePath(opts.srcPath, dir, title, ext);

                if (opts.srcFileType == MeshFileType::kAuto)
                {
                    // 自动格式，根据文件扩展名判断
                    if (ext.empty())
                    {
                        // 没扩展名，就无法判断了
                        ret = false;
                        break;
                    }
                    
                    opts.srcFileType = parseFileType(ext.c_str());
                }

                if (opts.srcFileType == opts.dstFileType)
                {
                    // 输入输出格式相同，不需要转换
                    MCONV_LOG_ERROR("Input and output file type is same. No need to convert.")
                    ret = false;
                    break;
                }

                if (opts.dstDir.empty())
                {
                    // 输出目录默认和输入文件目录相同
                    opts.dstDir = dir;
                }

                if (opts.dstTitle.empty())
                {
                    // 输出文件名默认和输入文件名相同
                    opts.dstTitle = title;
                }
            }
        } while (false);
        
        return ret;
    }

    //--------------------------------------------------------------------------

    void ConverterCommand::printCommand(int argc, char *argv[]) const
    {
        int i = 0;

        for (i = 1; i < argc; ++i)
        {
            if (i > 1)
                printf(" ");
            printf("%s", argv[i]);
        }

        printf("\n");
    }

    //--------------------------------------------------------------------------

    void ConverterCommand::printHelp() const
    {
        printf("Version : %s\n", MCONV_VERSION_STR);
        printf("Usage : ");
        printf("  mconv.exe [options] -o <type> <input full path> [<output directory>]\n");
        printf("    <input full path>  : The full path of the file to convert.\n");
        printf("    <output directory> : The directory of the output file.\n");
        printf("\n");
        printf("    [options] :\n");
        printf("      -? : Display this help information.\n");
        printf("      -i <type> : Set the type of the input file to <type>. The valid values is below\n");
        printf("          \"FBX\" - FBX(.fbx) file\n");
        printf("          \"OGRE\" - OGRE(*.mesh) file.\n");
        printf("          \"tiny3d\" - Tiny3D all skinned mesh data file including skinned mesh(*.tmesh), skeleton(*.tskel) and animation (*.tani) files.\n");
        printf("          \"tmesh\" - Tiny3D mesh (*.tmesh) file\n");
        printf("          \"tskin\" - Tiny3D skinned mesh (*.tmesh) file\n");
        printf("          \"tskel\" - Tiny3D skeleton (*.tskel) file\n");
        printf("          \"tani\" - Tiny3D animation (*.tani) file\n");
        printf("      -o <type> : Set the type of the output file to <type>. The valid values are below\n");
        printf("          \"FBX\" - FBX(.fbx) file\n");
        printf("          \"OGRE\" - OGRE(*.mesh) file.\n");
        printf("          \"tiny3d\" - Tiny3D all skinned mesh data file including skinned mesh(*.tmesh), skeleton(*.tskel) and animation (*.tani) files.\n");
        printf("          \"tmesh\" - Tiny3D mesh (*.tmesh) file\n");
        printf("          \"tskin\" - Tiny3D skinned mesh (*.tmesh) file\n");
        printf("          \"tskel\" - Tiny3D skeleton (*.tskel) file\n");
        printf("          \"tani\" - Tiny3D animation (*.tani) file\n");
        printf("      -t : Set the type of the output file to text or binary. Default is binary. It only take effect when output Tiny3D file.\n");
        printf("      -b <type> : Set the type of the bounding box to <type>. The valid values is below\n");
        printf("          \"sphere\" - Sphere Bounding Volume.\n");
        printf("          \"AABB\" - Axis Aligned Bounding Box. This is the default value.\n");
        printf("      -m <filename> : Set the material file when input file type is OGRE.\n");
        printf("      -d <filename> : Set the default material file path for engine.\n");
        printf("      -r <the root directory of resource> : Set the root of default material directory for engine.\n");
        printf("      -s <tiny3d compiled shader directory> : Set the compiled shader directory for engine. If not set this option, mconv will compile all shader lab.\n");
        printf("      -a : Generate the corresponding meta files.\n");
        printf("      -v : Verbose : print additional progress information\n");
        printf("\n");
    }

    //--------------------------------------------------------------------------

    MeshFileType ConverterCommand::parseFileType(const char *argv) const
    {
        MeshFileType type = MeshFileType::kFbx;

        if (_stricmp(argv, "fbx") == 0)
        {
            type = MeshFileType::kFbx;
        }
        else if (_stricmp(argv, "ogre") == 0)
        {
            type = MeshFileType::kOgre;
        }
        else if (_stricmp(argv, "tmesh") == 0)
        {
            type = MeshFileType::kTMesh;
        }
        else if (_stricmp(argv, "tskin") == 0)
        {
            type = MeshFileType::kTSkin;
        }
        else if (_stricmp(argv, "tskel") == 0)
        {
            type = MeshFileType::kTSkel;
        }
        else if (_stricmp(argv, "tani") == 0)
        {
            type = MeshFileType::kTAni;
        }
        else if (_stricmp(argv, "tiny3d") == 0)
        {
            type = MeshFileType::kTiny3D;
        }

        return type;
    }

    //--------------------------------------------------------------------------

    BoundType ConverterCommand::parseBoundType(const char *argv) const
    {
        BoundType type = BoundType::kSphere;

        if (_stricmp(argv, "sphere") == 0)
            type = BoundType::kSphere;
        else if (_stricmp(argv, "aabb") == 0)
            type = BoundType::kAabb;

        return type;
    }

    //--------------------------------------------------------------------------
}

