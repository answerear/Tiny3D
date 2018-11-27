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

#include "IntersectionApp.h"


using namespace Tiny3D;


IntersectionApp::IntersectionApp()
    : Application()
{
}

IntersectionApp::~IntersectionApp()
{
}


/*******************************************************************************
                                    Y
                                    |
                                    |
                                    |
                                    |
                                    |
                                    |
                                    |
                                    |
                                    |
                                    |______________________________ X
                                   / 
                                  /  O
                                 /
                                /
                               /
                              /
                             /
                            /
                           Z
 ******************************************************************************/
bool IntersectionApp::applicationDidFinishLaunching()
{
    // 朝向屏幕里面的射线
    Ray ray(Vector3::ZERO, Vector3::NEGATIVE_UNIT_Z * Real(1000));

    // AABB

    return true;
}

void IntersectionApp::applicationDidEnterBackground()
{
}

void IntersectionApp::applicationWillEnterForeground()
{
}

void IntersectionApp::applicationWillTerminate()
{

}


