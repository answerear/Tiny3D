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

#include "HelloApp.h"


using namespace Tiny3D;

HelloApp::HelloApp()
    : Application()
{
}

HelloApp::~HelloApp()
{
}

bool HelloApp::applicationDidFinishLaunching()
{
    return true;
}

void HelloApp::applicationDidEnterBackground()
{
    T3D_ENGINE.appDidEnterBackground();
}

void HelloApp::applicationWillEnterForeground()
{
    T3D_ENGINE.appWillEnterForeground();
}

void HelloApp::applicationWillTerminate()
{

}

void HelloApp::applicationLowMemory()
{

}

