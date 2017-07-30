/*******************************************************************************
 * This file is part of Mesh-converter (A mesh converter for Tiny3D Engine)
 * Copyright (C) 2015-2017  Answer Wong
 * For latest info, see https://github.com/asnwerear/Tiny3D
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

#include <stdio.h>
#include "mconv_converter.h"


int main(int argc, char *argv[])
{
    Tiny3D::System *pSystem = new Tiny3D::System();

    Tiny3D::Logger *pLogger = new Tiny3D::Logger();
    T3D_LOG_STARTUP(1001, "MeshConverter", true, false);

    T3D_LOG_INFO("Begin mesh-conv --------------------------------------");

    mconv::Converter *pConverter = new mconv::Converter();
    bool result = pConverter->execute(argc, argv);
    delete pConverter;

    T3D_LOG_INFO("End mesh-conv ----------------------------------------");

    T3D_LOG_SHUTDOWN();
    delete pLogger;

    delete pSystem;

	return (result ? 0 : -1);
}
