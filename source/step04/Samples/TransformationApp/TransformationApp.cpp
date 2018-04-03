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

#include "TransformationApp.h"


using namespace Tiny3D;


TransformationApp::TransformationApp()
    : Application()
{
}

TransformationApp::~TransformationApp()
{
}

bool TransformationApp::applicationDidFinishLaunching()
{
    Degree degree;
    Radian radian;

    String s = typeid(degree).name();
    T3D_LOG_INFO("TT %s", s.c_str());

    Real a = REAL_ZERO;

    a = Math::sin(radian);

    Vector2 p2(REAL_ONE, REAL_ZERO);

    Real scalar = REAL_ONE;
    Vector3 p3 = Vector3::UNIT_X;
    p3 = scalar * p3;
    Vector3 t = Vector3::UNIT_Y;
    t = p3.cross(t);

    Vector4 p4(REAL_ONE, REAL_ZERO, REAL_ZERO, REAL_ZERO);
    Vector4 t4(REAL_ZERO, REAL_ONE, REAL_ZERO, REAL_ZERO);
    t4 = p4.cross(t4);

    Matrix2 m2;
    p2 = m2 * p2;

    Matrix3 m3;
    p3 = m3 * p3;

    Quaternion q;
    q = scalar * q;

    Matrix4 m4;
    m4 = scalar * m4;
    p4 = m4 * p4;
    p4 = p4 * m4;

    return true;
}

void TransformationApp::applicationDidEnterBackground()
{
}

void TransformationApp::applicationWillEnterForeground()
{
}

void TransformationApp::applicationWillTerminate()
{

}

void TransformationApp::applicationLowMemory()
{

}

