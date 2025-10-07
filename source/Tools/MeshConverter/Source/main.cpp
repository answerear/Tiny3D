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


#include "T3DMeshConverterApp.h"

// For debug
// Text : -o tmesh -b sphere -t -r $(SolutionDir)..\..\assets\editor\builtin -d materials\Default-Material.tmat $(SolutionDir)..\..\assets\samples\raw\tortoise.fbx 


int main(int argc, char *argv[])
{
    using namespace Tiny3D;

    Dir::setCachePathInfo("Tiny3D", "MeshConverter");
    
    Agent *theEngine = T3D_NEW Agent();
    Settings settings;
    settings.pluginSettings.pluginPath = ".";
    settings.pluginSettings.plugins.push_back("FileSystemArchiveEditor");
    settings.pluginSettings.plugins.push_back("MetaFSArchive");
    settings.pluginSettings.plugins.push_back("FreeImageCodecEditor");
    settings.pluginSettings.plugins.push_back("NullRendererEditor");
    settings.renderSettings.renderer = "NullRenderer";
    TResult ret = theEngine->init(argc, argv, true, false, settings);
    if (T3D_SUCCEEDED(ret))
        theEngine->run();

    T3D_SAFE_DELETE(theEngine);

    return ret;
}

