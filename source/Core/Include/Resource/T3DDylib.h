/*******************************************************************************
 * This file is part of Tiny3D (Tiny 3D Graphic Rendering Engine)
 * Copyright (C) 2015-2017  Aaron Wong
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

#ifndef __T3D_DYLIB_H__
#define __T3D_DYLIB_H__


#include "T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Dylib : public Resource
    {
    public:
        static DylibPtr create(const String &name);

        virtual ~Dylib();

        virtual Type getType() const override;

        virtual void *getSymbol(const String &name) const;

    protected:
        Dylib(const String &name);

        virtual bool load() override;
        virtual void unload() override;
        virtual ResourcePtr clone() const override;

    protected:
        THandle   mHandle;
    };
}


#endif  /*__T3D_DYLIB_H__*/
