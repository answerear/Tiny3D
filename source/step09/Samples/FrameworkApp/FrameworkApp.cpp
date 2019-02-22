/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2019  Answer Wong
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

#include "FrameworkApp.h"
#include "Player.h"
#include "Enemy.h"


using namespace Tiny3D;

FrameworkApp::FrameworkApp()
    : Application()
    , mPlayer(nullptr)
    , mEnemy(nullptr)
{
}

FrameworkApp::~FrameworkApp()
{
}

bool FrameworkApp::applicationDidFinishLaunching()
{
    mPlayer = new Player("James");
    mEnemy = new Enemy("Kobe");

    mEnemy->idle();
    mPlayer->attack(mEnemy->getInstance());

    return true;
}

void FrameworkApp::applicationDidEnterBackground()
{
    T3D_AGENT.appDidEnterBackground();
}

void FrameworkApp::applicationWillEnterForeground()
{
    T3D_AGENT.appWillEnterForeground();
}

void FrameworkApp::applicationWillTerminate()
{
    T3D_SAFE_DELETE(mEnemy);
    T3D_SAFE_DELETE(mPlayer);
}

void FrameworkApp::applicationLowMemory()
{

}

