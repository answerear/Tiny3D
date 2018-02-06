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


#include "Player.h"


T3D_BEGIN_EVENT_FILTER(Player, MovingEntity)
    T3D_EVENT_FILTER(EV_MONSTER_ATTACK)
    T3D_EVENT_FILTER(EV_MONSTER_DEFEND)
    T3D_EVENT_FILTER(EV_MONSTER_ESCAPE)
T3D_END_EVENT_FILTER()

T3D_BEGIN_EVENT_MAP(Player, MovingEntity)
    T3D_ON_EVENT(EV_MONSTER_ATTACK, onMonsterAttack)
    T3D_ON_EVENT(EV_MONSTER_DEFEND, onMonsterDefend)
    T3D_ON_EVENT(EV_MONSTER_ESCAPE, onMonsterEscape)
T3D_END_EVENT_MAP()


Player::Player()
{
    T3D_SETUP_EVENT_FILTER();
}

Player::~Player()
{

}

void Player::onIdle()
{
    T3D_LOG_INFO("Player walking !!!");
}

int32_t Player::onMonsterAttack(EventParam *param, TINSTANCE sender)
{
    T3D_LOG_INFO("[Player] Oh shit !!! Monster attack me suddenly !!!");
    T3D_LOG_INFO("[Player] I defend !!!");
    postEvent(EV_PLAYER_DEFEND, nullptr);
    T3D_LOG_INFO("[Player] I attack monster !!!");
    postEvent(EV_PLAYER_ATTACK, nullptr);
    return T3D_ERR_OK;
}

int32_t Player::onMonsterDefend(EventParam *param, TINSTANCE sender)
{
    T3D_LOG_INFO("[Player] I continue to attack monster !!!");
    postEvent(EV_PLAYER_ATTACK, nullptr);
    return T3D_ERR_OK;
}

int32_t Player::onMonsterEscape(EventParam *param, TINSTANCE sender)
{
    T3D_LOG_INFO("[Player] Monster escape !!!");
    return T3D_ERR_OK;
}
