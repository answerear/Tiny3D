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


#include "T3DImageConverterCommand.h"
#include "T3DImageConverterOptions.h"
#include <algorithm>
#include <cctype>


namespace Tiny3D
{
    namespace
    {
        String toLowerStr(const String &s)
        {
            String out = s;
            std::transform(out.begin(), out.end(), out.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return out;
        }

        bool isOptionToken(const String &tok)
        {
            return tok.size() > 1 && tok[0] == '-';
        }

        void setError(String *outError, const String &msg)
        {
            if (outError != nullptr)
            {
                *outError = msg;
            }
            IMGCONV_LOG_ERROR("%s", msg.c_str());
        }
    }

    //--------------------------------------------------------------------------

    bool ImageConverterCommand::parse(int32_t argc, char *argv[], ImageConverterOptions &options, String *outError) const
    {
        StringArray args;
        for (int32_t i = 1; i < argc; ++i)
        {
            if (argv[i] != nullptr)
            {
                args.push_back(argv[i]);
            }
        }
        return parse(args, options, outError);
    }

    //--------------------------------------------------------------------------

    bool ImageConverterCommand::parse(const StringArray &args, ImageConverterOptions &options, String *outError) const
    {
        options = ImageConverterOptions();

        if (args.empty())
        {
            setError(outError, "No arguments specified.");
            return false;
        }

        for (size_t i = 0; i < args.size(); ++i)
        {
            const String &arg = args[i];

            if (arg == "-?" || arg == "-h" || arg == "--help")
            {
                options.showHelp = true;
                return true;
            }
            else if (arg == "-i")
            {
                if (i + 1 >= args.size() || isOptionToken(args[i + 1]))
                {
                    setError(outError, "Missing file path after -i.");
                    return false;
                }
                while (i + 1 < args.size() && !isOptionToken(args[i + 1]))
                {
                    options.srcPaths.push_back(args[++i]);
                }
            }
            else if (arg == "-o")
            {
                if (i + 1 >= args.size())
                {
                    setError(outError, "Missing output path after -o.");
                    return false;
                }
                options.dstFilePath = args[++i];
            }
            else if (arg == "-O")
            {
                if (i + 1 >= args.size())
                {
                    setError(outError, "Missing output directory after -O.");
                    return false;
                }
                options.dstDir = args[++i];
            }
            else if (arg == "-t")
            {
                if (i + 1 >= args.size() || isOptionToken(args[i + 1]))
                {
                    // 单独的 -t 表示文本(JSON)输出模式
                    options.isTextMode = true;
                }
                else
                {
                    String val = toLowerStr(args[++i]);
                    options.hasTextureType = true;
                    if (val == "default")
                    {
                        options.textureType = TextureImportType::kDefault;
                    }
                    else if (val == "ui")
                    {
                        options.textureType = TextureImportType::kUI;
                    }
                    else if (val == "normal")
                    {
                        options.textureType = TextureImportType::kNormalMap;
                    }
                    else
                    {
                        setError(outError, "Invalid texture type '" + val + "' (expected default|ui|normal).");
                        return false;
                    }
                }
            }
            else if (arg == "--type")
            {
                if (i + 1 >= args.size())
                {
                    setError(outError, "Missing texture type after --type.");
                    return false;
                }
                String val = toLowerStr(args[++i]);
                options.hasTextureType = true;
                if (val == "default")
                {
                    options.textureType = TextureImportType::kDefault;
                }
                else if (val == "ui")
                {
                    options.textureType = TextureImportType::kUI;
                }
                else if (val == "normal")
                {
                    options.textureType = TextureImportType::kNormalMap;
                }
                else
                {
                    setError(outError, "Invalid texture type '" + val + "' (expected default|ui|normal).");
                    return false;
                }
            }
            else if (arg == "--text")
            {
                options.isTextMode = true;
            }
            else if (arg == "-a")
            {
                options.isGeneratingMeta = true;
            }
            else if (arg == "-u")
            {
                if (i + 1 >= args.size())
                {
                    setError(outError, "Missing UUID policy after -u.");
                    return false;
                }
                String val = toLowerStr(args[++i]);
                if (val == "reuse")
                {
                    options.uuidPolicy = ImageConverterOptions::UUIDPolicy::kReuse;
                }
                else if (val == "new")
                {
                    options.uuidPolicy = ImageConverterOptions::UUIDPolicy::kNew;
                }
                else
                {
                    setError(outError, "Invalid UUID policy '" + val + "' (expected reuse|new).");
                    return false;
                }
            }
            else if (arg == "--uuid")
            {
                if (i + 1 >= args.size())
                {
                    setError(outError, "Missing UUID value after --uuid.");
                    return false;
                }
                String hex = args[++i];
                UUID parsed;
                parsed.fromString(hex);
                if (parsed == UUID::INVALID)
                {
                    setError(outError, "Invalid UUID hex string '" + hex + "'.");
                    return false;
                }
                options.explicitUUID = parsed;
            }
            else if (arg == "--premultiply")
            {
                options.hasPremultipliedAlpha = true;
                options.premultipliedAlpha = true;
            }
            else if (arg == "--no-premultiply")
            {
                options.hasPremultipliedAlpha = true;
                options.premultipliedAlpha = false;
            }
            else if (arg == "--mips")
            {
                options.hasGenerateMipmaps = true;
                options.generateMipmaps = true;
            }
            else if (arg == "--no-mips")
            {
                options.hasGenerateMipmaps = true;
                options.generateMipmaps = false;
            }
            else if (arg == "--flip-y")
            {
                options.hasFlipY = true;
                options.flipY = true;
            }
            else if (arg == "--no-flip-y")
            {
                options.hasFlipY = true;
                options.flipY = false;
            }
            else if (arg == "--max-size")
            {
                if (i + 1 >= args.size())
                {
                    setError(outError, "Missing integer value after --max-size.");
                    return false;
                }
                String val = args[++i];
                try
                {
                    unsigned long sz = std::stoul(val);
                    if (sz == 0)
                    {
                        setError(outError, "--max-size must be > 0.");
                        return false;
                    }
                    options.hasMaxSize = true;
                    options.maxSize = static_cast<uint32_t>(sz);
                }
                catch (...)
                {
                    setError(outError, "Invalid --max-size value '" + val + "'.");
                    return false;
                }
            }
            else if (arg == "--filter")
            {
                if (i + 1 >= args.size())
                {
                    setError(outError, "Missing filter mode after --filter.");
                    return false;
                }
                String val = toLowerStr(args[++i]);
                options.hasFilter = true;
                if (val == "point")
                {
                    options.filter = FilterOptions::kPoint;
                }
                else if (val == "linear")
                {
                    options.filter = FilterOptions::kLinear;
                }
                else
                {
                    setError(outError, "Invalid filter mode '" + val + "' (expected point|linear).");
                    return false;
                }
            }
            else if (arg == "--wrap")
            {
                if (i + 1 >= args.size())
                {
                    setError(outError, "Missing wrap mode after --wrap.");
                    return false;
                }
                String val = toLowerStr(args[++i]);
                options.hasWrap = true;
                if (val == "repeat" || val == "wrap")
                {
                    options.wrap = TextureAddressMode::kWrap;
                }
                else if (val == "clamp")
                {
                    options.wrap = TextureAddressMode::kClamp;
                }
                else if (val == "mirror")
                {
                    options.wrap = TextureAddressMode::kMirror;
                }
                else if (val == "border")
                {
                    options.wrap = TextureAddressMode::kBorder;
                }
                else
                {
                    setError(outError, "Invalid wrap mode '" + val + "' (expected repeat|clamp|mirror|border).");
                    return false;
                }
            }
            else if (arg == "-f")
            {
                options.forceRebuild = true;
            }
            else if (arg == "-v")
            {
                options.verbose = true;
            }
            else if (arg == "--ipc-port")
            {
                if (i + 1 >= args.size())
                {
                    setError(outError, "Missing port number after --ipc-port.");
                    return false;
                }
                String val = args[++i];
                try
                {
                    unsigned long p = std::stoul(val);
                    if (p == 0 || p > 65535)
                    {
                        setError(outError, "Invalid --ipc-port '" + val + "'.");
                        return false;
                    }
                    options.ipcMode = true;
                    options.ipcPort = static_cast<uint16_t>(p);
                }
                catch (...)
                {
                    setError(outError, "Invalid --ipc-port '" + val + "'.");
                    return false;
                }
            }
            else if (arg == "--ipc-token")
            {
                if (i + 1 >= args.size())
                {
                    setError(outError, "Missing token after --ipc-token.");
                    return false;
                }
                options.ipcToken = args[++i];
            }
            else if (!isOptionToken(arg))
            {
                // 兼容位置参数作为输入文件
                options.srcPaths.push_back(arg);
            }
            else
            {
                setError(outError, "Unknown option '" + arg + "'.");
                return false;
            }
        }

        if (options.ipcMode)
        {
            if (options.ipcPort == 0 || options.ipcToken.empty())
            {
                setError(outError, "IPC mode requires both --ipc-port <port> and --ipc-token <token>.");
                return false;
            }
            return true;
        }

        if (options.srcPaths.empty())
        {
            setError(outError, "No input files specified (-i <file> ...).");
            return false;
        }

        if (!options.dstFilePath.empty() && !options.dstDir.empty())
        {
            setError(outError, "Options -o and -O cannot be used together.");
            return false;
        }

        if (!options.dstFilePath.empty() && options.srcPaths.size() > 1)
        {
            setError(outError, "Option -o can only be used with a single input file (use -O for batch output).");
            return false;
        }

        if (options.explicitUUID != UUID::INVALID && options.srcPaths.size() > 1)
        {
            setError(outError, "Option --uuid can only be used with a single input file.");
            return false;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    void ImageConverterCommand::printHelp() const
    {
        printf("ImageConverter (imgconv) Version %s\n", IMGCONV_VERSION_STR);
        printf("Usage:\n");
        printf("  imgconv [options] -i <file1> [file2 ...]\n");
        printf("  imgconv --ipc-port <port> --ipc-token <token>\n\n");
        printf("Options:\n");
        printf("  -i <f1> [f2 ...]        Input image file path(s) (PNG / JPG / BMP / TGA / DDS / HDR / EXR ...)\n");
        printf("  -o <path>               Output .ttex file path (single input only)\n");
        printf("  -O <dir>                Output directory for batch conversion\n");
        printf("  -t, --type <type>       Texture import type: default | ui | normal (default: default)\n");
        printf("  --text                  Export .ttex in JSON text format (default: T3DB binary)\n");
        printf("  -a                      Generate paired <name>.ttex.meta file\n");
        printf("  -u <policy>             UUID policy when .meta exists: reuse | new (default: reuse)\n");
        printf("  --uuid <hex32>          Explicitly set UUID for single texture\n");
        printf("  --premultiply           Force premultiplied alpha\n");
        printf("  --no-premultiply        Disable premultiplied alpha\n");
        printf("  --mips                  Force mipmap chain generation\n");
        printf("  --no-mips               Disable mipmap chain generation (mipmaps = 1)\n");
        printf("  --flip-y                Flip image vertically before export\n");
        printf("  --no-flip-y             Do not flip image vertically\n");
        printf("  --max-size <n>          Maximum texture dimension (default: 4096)\n");
        printf("  --filter <mode>         Sampler filter: point | linear\n");
        printf("  --wrap <mode>           Sampler address mode: repeat | clamp | mirror | border\n");
        printf("  -f                      Force rebuild (ignore SourceHash incremental check)\n");
        printf("  -v                      Verbose output\n");
        printf("  -? / -h                 Show this help message\n");
    }
}
