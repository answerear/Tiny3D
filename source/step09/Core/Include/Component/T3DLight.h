/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2020  Answer Wong
 * For latest info, see https://github.com/answerear/Tiny3D
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


#ifndef __T3D_LIGHT_H__
#define __T3D_LIGHT_H__


#include "Component/T3DTransform3D.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Light : public Transform3D
    {
    public:
        static LightPtr create(ID uID = E_CID_AUTOMATIC);

        virtual ~Light();

        virtual const String &getType() const override;

    protected:
        Light(ID uID = E_CID_AUTOMATIC);

        virtual ComponentPtr clone() const override;

        virtual TResult cloneProperties(ComponentPtr newObj) const override;

    };
}


#endif  /*__T3D_LIGHT_H__*/
