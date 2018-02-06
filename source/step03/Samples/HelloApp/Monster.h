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

#ifndef __MONSTER_H__
#define __MONSTER_H__


#include "MovingEntity.h"


class Monster : public MovingEntity
{
    T3D_DECLARE_EVENT_MAP();
    T3D_DECLARE_EVENT_FILTER();

public:
    Monster();
    virtual ~Monster();

protected:
    virtual void onIdle() override;

    T3D_DECLARE_EVENT_HANDLE(onPlayerAttack);
    T3D_DECLARE_EVENT_HANDLE(onPlayerDefend);

    int32_t mAttackCount;
};


#endif  /*__MONSTER_H__*/
