

#include "mconv_command.h"


namespace mconv
{
    bool Command::parse(int argc, char *argv[], Settings &settings)
    {
        printCommand(argc, argv);

        if (argc < 3)
        {
            printHelp();
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
}
