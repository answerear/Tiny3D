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



// For Test
// -h
// -i "D:\private\Tiny3D\assets\samples\ui\badge.png" -o "D:\private\Tiny3D\assets\samples\ui\badge.ttex" -t ui -a -f
// -i "D:\private\Tiny3D\assets\samples\ui\badge.png" -o "D:\private\Tiny3D\assets\samples\ui\badge.ttex" -t ui -a


#include "T3DImageConverterApp.h"
#include "T3DImageConverterCommand.h"


Tiny3D::ImageConverterApp theApp;


int main(int argc, char *argv[])
{
    using namespace Tiny3D;

    // 先快速检查是否为空参或帮助，避免在未给参数时等待引擎/插件初始化
    if (argc <= 1)
    {
        ImageConverterCommand cmd;
        fprintf(stderr, "Error: No arguments specified.\n\n");
        cmd.printHelp();
        return 2;
    }
    for (int i = 1; i < argc; ++i)
    {
        String arg = argv[i] ? argv[i] : "";
        if (arg == "-?" || arg == "-h" || arg == "--help")
        {
            ImageConverterCommand cmd;
            cmd.printHelp();
            return 0;
        }
    }

    Dir::setCachePathInfo("Tiny3D", "ImageConverter");

    Agent *theEngine = T3D_NEW Agent();
    Settings settings;
    settings.pluginSettings.pluginPath = ".";
    // 注意：不要挂载 MetaFSArchiveEditor，由 TextureExporter 显式写出含 SourceHash 和 ImportSettings 的 MetaTexture
    settings.pluginSettings.plugins.push_back("FileSystemArchiveEditor");
    settings.pluginSettings.plugins.push_back("FreeImageCodecEditor");
    settings.pluginSettings.plugins.push_back("NullRendererEditor");
    settings.renderSettings.renderer = RHIRenderer::NULLRENDERER;

    TResult ret = theEngine->init(argc, argv, true, false, settings);
    if (T3D_FAILED(ret))
    {
        T3D_SAFE_DELETE(theEngine);
        return (theApp.getExitCode() != 0) ? theApp.getExitCode() : 2;
    }

    T3D_SAFE_DELETE(theEngine);
    return theApp.getExitCode();
}
