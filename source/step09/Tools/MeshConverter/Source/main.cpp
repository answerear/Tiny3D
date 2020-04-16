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


#include "T3DMeshConverter.h"

// For debug
// "$(SolutionDir)..\..\..\assets\scripts\BuiltinBox.material" -t hlsl -m 50
// BuiltinBox.material BuiltinConstant.constant BuiltinProgram.program -t hlsl -m 50 -O3 -p $(SolutionDir)..\..\..\assets\scripts\
// materials/BuiltinNoTexture.material program/BuiltinConstant.constant program/BuiltinProgram.program -t hlsl -m 50 -O3 -p $(SolutionDir)..\..\..\assets\builtin


int main(int argc, char *argv[])
{
    using namespace Tiny3D;

    System *system = new System();

    Logger *logger = new Logger();
    logger->startup(1001, "MeshConverter", true, false);

    T3D_LOG_INFO(TAG_MESH_CONVERTER, 
        "Begin mesh-conv --------------------------------------");

    MeshConverter *converter = new MeshConverter();
    TResult ret = converter->execute(argc, argv);
    delete converter;

    T3D_LOG_INFO(TAG_MESH_CONVERTER,
        "End mesh-conv ----------------------------------------");

    logger->shutdown();

    delete logger;
    delete system;

    return ret;
}

