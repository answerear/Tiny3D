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


#ifndef __ENTITY_H__
#define __ENTITY_H__


#include <Tiny3D.h>

using namespace Tiny3D;

enum Event
{
    EV_ATTACKED = 1,
    EV_CHANGE_STATE,
};

class Entity : public EventHandler
{
public:
    Entity();
    virtual ~Entity();

    virtual void attack() = 0;
    virtual void defend() = 0;
    virtual void walk() = 0;
    virtual void idle() = 0;

protected:
    bool onAttacked(EventParam *param, TINSTANCE sender);
};


#endif  /*__ENTITY_H__*/

