

#include "mconv_command.h"
#include "mconv_settings.h"


namespace mconv
{
    bool Command::parse(int argc, char *argv[], Settings &settings)
    {
        printCommand(argc, argv);

        if (argc < 3)
        {
            printHelp();
            return false;
        }

        bool bShowHelp = false;
        settings.mVerbose = false;
        String ext;

        int i = 0;
        for (i = 1; i < argc; ++i)
        {
            const char *arg = argv[i];
            int len = strlen(arg);
            if (len > 1 && arg[0] == '-')
            {
                if (arg[1] == '?')
                {
                    bShowHelp = true;
                }
                else if (arg[1] == 'v')
                {
                    settings.mVerbose = true;
                }
                else if (arg[1] == 'i')
                {
                    settings.mSrcType = parseType(argv[++i]);
                }
                else if (arg[1] == 'o')
                {
                    settings.mDstType = parseType(argv[++i]);
                    ext = argv[i];
                }
            }
            else if (settings.mSrcPath.length() == 0)
            {
                settings.mSrcPath = arg;
            }
            else if (settings.mDstPath.length() == 0)
            {
                settings.mDstPath = arg;
            }
        }

        if (bShowHelp)
        {
            printHelp();
            return false;
        }

        if (settings.mDstPath.length() == 0)
        {
            
        }

        return true;
    }

    void Command::printCommand(int argc, char *argv[]) const
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

    void Command::printHelp() const
    {
        printf("Usage: mesh-conv.exe [options] <input> [<output>]\n");
        printf("\n");
        printf("Options:\n");
        printf("-?       : Display this help information.\n");
        printf("-i <type>: Set the type of the input file to <type>\n");
        printf("\t<type> : This type should be FBX (fbx), T3DB (t3db), T3DT (t3dt) or DAE (dae).\n");
        printf("-o <type>: Set the type of the output file to <type>\n");
        printf("\t<type> : This type should be FBX (fbx), T3DB (t3db), T3DT (t3dt), T3D (t3d) or DAE (dae).\n");
        printf("-v       : Verbose: print additional progress information\n");
        printf("\n");
        printf("<input>  : The filename of the file to convert.\n");
        printf("<output> : The filename of the converted file.\n");
        printf("\n");
        printf("<type>   : FBX, T3DB (binary) or T3DT (xml).\n");
    }

    FileType Command::parseType(const char *arg) const
    {
        FileType type = E_FILETYPE_AUTO;

        if (stricmp(arg, "fbx") == 0)
            type = E_FILETYPE_FBX;
        else if (stricmp(arg, "t3db") == 0)
            type = E_FILETYPE_T3DB;
        else if (stricmp(arg, "t3dt") == 0)
            type = E_FILETYPE_T3DT;
        else if (stricmp(arg, "t3d") == 0)
            type = E_FILETYPE_T3D;

        return type;
    }
}
