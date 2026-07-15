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


#include "T3DBundleBuilderApp.h"

// For debug
// 用法 : --assets <资源目录> [--assets <附加目录> ...] --out <输出目录>
// 例子 : --assets $(SolutionDir)..\..\assets\samples --assets $(SolutionDir)..\..\Temp\shaders --out $(SolutionDir)..\..\assets\samples\bundle
// 例子 : --assets $(SolutionDir)..\..\assets\editor\builtin --out $(SolutionDir)..\..\assets\editor\builtin\bundle
// For Test : --assets $(SolutionDir)..\..\assets\samples\meshes --out $(SolutionDir)..\..\assets\samples\bundle --binary --verify


int main(int argc, char *argv[])
{
    using namespace Tiny3D;

    Dir::setCachePathInfo("Tiny3D", "BundleBuilder");

    Agent *theEngine = T3D_NEW Agent();

    Settings settings;
    settings.pluginSettings.pluginPath = ".";
    settings.pluginSettings.plugins.push_back("FileSystemArchiveEditor");
    settings.pluginSettings.plugins.push_back("MetaFSArchiveEditor");
    settings.pluginSettings.plugins.push_back("NullRendererEditor");
    settings.renderSettings.renderer = RHIRenderer::NULLRENDERER;

    TResult ret = theEngine->init(argc, argv, true, false, settings);
    if (T3D_SUCCEEDED(ret))
        theEngine->run();

    T3D_SAFE_DELETE(theEngine);

    return ret;
}
