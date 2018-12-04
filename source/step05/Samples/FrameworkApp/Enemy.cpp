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


#include "Enemy.h"

T3D_BEGIN_EVENT_FILTER(Enemy, Entity)
T3D_END_EVENT_FILTER()

T3D_BEGIN_EVENT_MAP(Enemy, Entity)
T3D_END_EVENT_MAP()


Enemy::Enemy(const String &name)
    : Entity(name)
{

}

Enemy::~Enemy()
{

}

void Enemy::fire(TINSTANCE instance)
{
    mState = ST_FIRING;
    T3D_LOG_INFO("Enemy [%s] fire skill. State [%d]", mName.c_str(), mState);
}

void Enemy::attack(TINSTANCE instance)
{
    mState = ST_ATTACKING;
    T3D_LOG_INFO("Enemy [%s] attack !", mName.c_str(), mState);
}

void Enemy::defend(TINSTANCE attacker)
{

}

void Enemy::idle()
{

}

