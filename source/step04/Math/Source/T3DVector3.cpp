/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
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

#include "T3DVector3.h"


namespace Tiny3D
{
    const Vector3 Vector3::ZERO(0.0, 0.0, 0.0);
    const Vector3 Vector3::UNIT_SCALE(1.0, 1.0, 1.0);
    const Vector3 Vector3::UNIT_X(1.0, 0.0, 0.0);
    const Vector3 Vector3::UNIT_Y(0.0, 1.0, 0.0);
    const Vector3 Vector3::UNIT_Z(0.0, 0.0, 1.0);
    const Vector3 Vector3::NEGATIVE_UNIT_X(-1.0, 0.0, 0.0);
    const Vector3 Vector3::NEGATIVE_UNIT_Y(0.0, -1.0, 0.0);
    const Vector3 Vector3::NEGATIVE_UNIT_Z(0.0, 0.0, -1.0);
}
