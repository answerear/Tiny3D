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

#include "T3DSCCPrerequisites.h"
#include "T3DShaderCross.h"


// For debug
// "$(SolutionDir)..\..\..\assets\scripts\BuiltinBox.material" -t hlsl -m 50
// BuiltinBox.material BuiltinConstant.constant BuiltinProgram.program -t hlsl -m 50 -O3 -p $(SolutionDir)..\..\..\assets\scripts\
// materials/BuiltinNoTexture.material program/BuiltinConstant.constant program/BuiltinProgram.program -t hlsl -m 50 -O3 -b -p $(SolutionDir)..\..\..\assets\builtin

// For debug
// "$(SolutionDir)..\..\..\assets\builtin\materials\Test.Shader" -t hlsl

int main(int argc, char *argv[])
{
    using namespace Tiny3D;

    Platform* platform = new Platform();
    Logger *logger = new Logger();
    T3D_LOG_STARTUP(1000, LOG_TAG, true, true);

    ObjectTracer* tracer = new ObjectTracer(true);

    ShaderCrossPtr sc = ShaderCross::create();

    int ret = 0;

    do 
    {
        if (argc == 1)
        {
            T3D_LOG_ERROR(LOG_TAG, "Not enough parameters !");
            ret = -1;
            break;
        }

        if (!sc->compile(argc-1, (const char**)(argv+1)))
        {
            ret = -2;
            break;
        }

        ret = 0;
    } while (0);

    sc = nullptr;
    delete tracer;

    T3D_LOG_SHUTDOWN();
    delete logger;
    delete platform;

    return ret;
}

