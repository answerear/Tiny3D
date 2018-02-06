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


#include "Scene.h"
#include "Player.h"
#include "Monster.h"


Scene::Scene()
    : mTimerID(T3D_INVALID_TIMER_ID)
    , mPlayer(nullptr)
    , mMonster(nullptr)
{

}

Scene::~Scene()
{
    if (mTimerID != T3D_INVALID_TIMER_ID)
    {
        T3D_TIMER_MGR.stopTimer(mTimerID);
        mTimerID = T3D_INVALID_TIMER_ID;
    }

    T3D_SAFE_DELETE(mPlayer);
    T3D_SAFE_DELETE(mMonster);
}

void Scene::init()
{
    mPlayer = new Player();
    mMonster = new Monster();

    mTimerID = T3D_TIMER_MGR.startTimer(2000, false, this);
}

void Scene::onTimer(uint32_t timerID, int32_t dt)
{
    if (timerID == mTimerID)
    {
        postEvent(EV_ENTITY_MOVE, nullptr);
    }
}

