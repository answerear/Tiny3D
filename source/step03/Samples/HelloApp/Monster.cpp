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


#include "Monster.h"

T3D_BEGIN_EVENT_FILTER(Monster, MovingEntity)
    T3D_EVENT_FILTER(EV_PLAYER_ATTACK)
    T3D_EVENT_FILTER(EV_PLAYER_DEFEND)
T3D_END_EVENT_FILTER()


T3D_BEGIN_EVENT_MAP(Monster, MovingEntity)
    T3D_ON_EVENT(EV_PLAYER_ATTACK, onPlayerAttack)
    T3D_ON_EVENT(EV_PLAYER_DEFEND, onPlayerDefend)
T3D_END_EVENT_MAP()


Monster::Monster()
    : mAttackCount(0)
{
    T3D_SETUP_EVENT_FILTER();
}

Monster::~Monster()
{

}

void Monster::onIdle()
{
    mAttackCount = 0;
    T3D_LOG_INFO("[Monster] Hey, I am monster. I am idle. I am attacking player !!!");
    postEvent(EV_MONSTER_ATTACK, nullptr);
}

int32_t Monster::onPlayerAttack(EventParam *param, TINSTANCE sender)
{
    T3D_LOG_INFO("[Monster] Oh shit !!! Player attacked me #%d, I must escape !!!", mAttackCount);
    mAttackCount++;
    if (mAttackCount <= 1)
        postEvent(EV_MONSTER_DEFEND, nullptr);
    else
        postEvent(EV_MONSTER_ESCAPE, nullptr);
    return T3D_ERR_OK;
}

int32_t Monster::onPlayerDefend(EventParam *param, TINSTANCE sender)
{
    T3D_LOG_INFO("[Monster] What the fuck !!! Player defended !!! I start walking.");
    postEvent(EV_ENTITY_MOVE, nullptr, getInstance());
    return T3D_ERR_OK;
}

