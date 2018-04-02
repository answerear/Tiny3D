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

#include "T3DMath.h"


namespace Tiny3D
{
    const Real Math::POS_INFINITY = std::numeric_limits<Real>::infinity();
    const Real Math::NEG_INFINITY = -std::numeric_limits<Real>::infinity();

    const Real Math::PI = Real(4.0f * ::atan(1.0));
    const Real Math::TWO_PI = Real(2.0f * PI);
    const Real Math::HALF_PI = Real(0.5f * PI);

    const Real Math::DEGREES_TO_RADIANS = PI / Real(180.0f);
    const Real Math::RADIANS_TO_DEGREES = Real(180.0f) / PI;
}
