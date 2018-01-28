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
//#include <windows.h>

HelloApp::HelloApp()
    : Application()
{
//    ::AllocConsole();
//    freopen("CONIN$", "r+t", stdin);
//    freopen("CONOUT$", "w+t", stdout);
}

HelloApp::~HelloApp()
{
//    ::FreeConsole();
}

bool HelloApp::applicationDidFinishLaunching()
{
    Tiny3D::DateTime current;
    int64_t t = current.currentMSecsSinceEpoch();
    printf("Epoch ms : %lld\n", t);
    t = current.currentSecsSinceEpoch();
    printf("Epoch s : %lld\n", t);
    Tiny3D::DateTime time = Tiny3D::DateTime::currentDateTime();
    printf("%s_%s\n", time.dateToString().c_str(), time.timeToString().c_str());
    Tiny3D::DateTime timeUTC = Tiny3D::DateTime::currentDateTimeUTC();
    printf("UTC %s_%s", timeUTC.dateToString().c_str(), timeUTC.timeToString().c_str());

    return true;
}

void HelloApp::applicationDidEnterBackground()
{

}

void HelloApp::applicationWillEnterForeground()
{

}

void HelloApp::applicationWillTerminate()
{

}

