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

#ifndef __T3D_PREFAB_H__
#define __T3D_PREFAB_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Prefab : public Resource
    {
    public:
        static PrefabPtr create(const String &name);

        ~Prefab() override;

        Type getType() const override;
        
        template<typename T>
        T *instantiateAsPointer()
        {
            T *src = mObject->try_convert<T>();
            return src->clone();
        }

        template<typename T>
        T instantiateAsObject()
        {
            T *src = mObject->try_convert<T>();
            return *src;
        }
        
    protected:
        Prefab(const String &name);
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;
        
        RTTRObject  *mObject;
    };
}


#endif    /*__T3D_PREFAB_H__*/
