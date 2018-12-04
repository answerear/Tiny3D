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

T3D_BEGIN_EVENT_FILTER(Player, Entity)
T3D_END_EVENT_FILTER()

T3D_BEGIN_EVENT_MAP(Player, Entity)
T3D_END_EVENT_MAP()


Player::Player(const String &name)
    : Entity(name)
{

}

Player::~Player()
{

}

void Player::fire(TINSTANCE target)
{

}

void Player::attack(TINSTANCE target)
{

}

void Player::defend(TINSTANCE attacker)
{

}

void Player::idle()
{

}
