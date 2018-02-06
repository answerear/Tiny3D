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


#include "MovingEntity.h"

T3D_BEGIN_EVENT_FILTER(MovingEntity, EventHandler)
    T3D_EVENT_FILTER(EV_ENTITY_MOVE)
T3D_END_EVENT_FILTER()

T3D_BEGIN_EVENT_MAP(MovingEntity, EventHandler)
    T3D_ON_EVENT(EV_ENTITY_MOVE, onMove)
T3D_END_EVENT_MAP()


MovingEntity::MovingEntity()
    : mWalkTimerID(T3D_INVALID_TIMER_ID)
{
    T3D_SETUP_EVENT_FILTER();
}

MovingEntity::~MovingEntity()
{
    if (mWalkTimerID != T3D_INVALID_TIMER_ID)
    {
        T3D_TIMER_MGR.stopTimer(mWalkTimerID);
    }
}

void MovingEntity::onTimer(uint32_t timerID, int32_t dt)
{
    if (timerID == mWalkTimerID)
    {
        mWalkTimerID = T3D_INVALID_TIMER_ID;
        onIdle();
    }
}

int32_t MovingEntity::onMove(EventParam *param, TINSTANCE sender)
{
    // 开始行走5秒
    T3D_LOG_INFO("Moving entity start walking for 5 seconds......");
    mWalkTimerID = T3D_TIMER_MGR.startTimer(5000, false, this);
    return T3D_ERR_OK;
}

