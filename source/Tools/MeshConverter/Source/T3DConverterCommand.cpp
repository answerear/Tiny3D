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

    bool ConverterCommand::parse(int32_t argc, char *argv[], 
        ConverterOptions &options)
    {
        printCommand(argc, argv);

        if (argc < 3)
        {
            printHelp();
            return false;
        }

        bool bShowHelp = false;
        String ext;

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
                    options.verbose = true;
                }
                else if (arg[1] == 't')
                {
                    options.isTxt = true;
                }
                else if (arg[1] == 'i')
                {
                    options.srcFileType = parseFileType(argv[++i]);
                }
                else if (arg[1] == 'o')
                {
                    options.dstFileType = parseFileType(argv[++i]);
                    ext = argv[i];
                }
                else if (arg[1] == 'b')
                {
                    options.boundType = parseBoundType(argv[++i]);
                }
                else if (arg[1] == 'm')
                {
                    options.extraPath = argv[++i];
                }
            }
            else if (options.srcPath.empty())
            {
                options.srcPath = arg;
            }
            else if (options.dstPath.empty())
            {
                options.dstPath = arg;
            }
        }

        if (bShowHelp || options.srcPath.empty())
        {
            printHelp();
            return false;
        }

        if (options.srcFileType == MeshFileType::kAuto)
        {
            const String srcPath = options.srcPath;
            String::size_type pos = srcPath.rfind('.');
            if (pos != String::npos)
            {
                ext = srcPath.substr(pos + 1);
                options.srcFileType = parseFileType(ext.c_str());
            }
        }

        if (options.dstPath.empty())
        {
            const String &srcPath = options.srcPath;
            String::size_type pos = srcPath.rfind('.');
            options.dstPath = srcPath.substr(0, pos);

            switch (options.dstFileType)
            {
            case MeshFileType::kFbx:
                options.dstPath += ".fbx";
                break;
            case MeshFileType::kOgre:
                options.dstPath += ".mesh";
                break;
            case MeshFileType::kTMesh:
                options.dstPath += Resource::EXT_MESH;
                break;
            case MeshFileType::kAuto:
            default:
                T3D_ASSERT(false);
                break;
            }
        }

        return true;
    }

    //--------------------------------------------------------------------------

    void ConverterCommand::printCommand(int argc, char *argv[]) const
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

    void ConverterCommand::printHelp() const
    {
        printf("Version : %s\n", MCONV_VERSION_STR);
        printf("Usage : ");
        printf("  mconv.exe [options] <input> [<output>]\n");
        printf("    <input>  : The filename of the file to convert.\n");
        printf("    <output> : The filename of the converted file.\n");
        printf("\n");
        printf("    Options:\n");
        printf("      -? : Display this help information.\n");
        printf("      -i <type> : Set the type of the input file to <type>. The valid values is below\n");
        printf("          \"FBX\" - FBX(.fbx) file\n");
        printf("          \"OGRE\" - OGRE(*.mesh) file.\n");
        printf("          \"tmesh\" - Tiny3D mesh or skinned mesh (*.tmesh) file\n");
        printf("          \"tskel\" - Tiny3D skeleton (*.tskel) file\n");
        printf("          \"tani\" - Tiny3D animation (*.tani) file\n");
        printf("      -o <type> : Set the type of the output file to <type>. The valid values is below\n");
        printf("          \"FBX\" - FBX(.fbx) file\n");
        printf("          \"OGRE\" - OGRE(*.mesh) file.\n");
        printf("          \"tmesh\" - Tiny3D mesh or skinned mesh (*.tmesh) file\n");
        printf("          \"tskel\" - Tiny3D skeleton (*.tskel) file\n");
        printf("          \"tani\" - Tiny3D animation (*.tani) file\n");
        printf("      -t : Set the type of the output file to text or binary. Default is binary. It only take effect when output Tiny3D file.\n");
        printf("      -b <type> : Set the type of the bounding box to <type>. The valid values is below\n");
        printf("          \"sphere\" - Sphere Bounding Volume\n");
        printf("          \"AABB\" - Axis Aligned Bounding Box\n");
        printf("      -m <filename> : Set the material file when input file type is OGRE.\n");
        printf("      -v : Verbose : print additional progress information\n");
        printf("\n");
    }

    //--------------------------------------------------------------------------

    MeshFileType ConverterCommand::parseFileType(const char *argv) const
    {
        MeshFileType type = MeshFileType::kFbx;

        if (stricmp(argv, "fbx") == 0)
            type = MeshFileType::kFbx;
        else if (stricmp(argv, "ogre") == 0)
            type = MeshFileType::kOgre;
        else if (stricmp(argv, "tmesh") == 0)
            type = MeshFileType::kTMesh;

        return type;
    }

    //--------------------------------------------------------------------------

    BoundType ConverterCommand::parseBoundType(const char *argv) const
    {
        BoundType type = BoundType::kSphere;

        if (stricmp(argv, "sphere") == 0)
            type = BoundType::kSphere;
        else if (stricmp(argv, "aabb") == 0)
            type = BoundType::kAabb;

        return type;
    }

    //--------------------------------------------------------------------------
}

