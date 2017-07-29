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

#ifndef __T3D_RESOURCE_H__
#define __T3D_RESOURCE_H__


#include "Misc/T3DObject.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Resource : public Object
    {
        friend class ResourceManager;

    public:
        enum Type
        {
            E_TYPE_UNKNOWN = 0,
            E_TYPE_DYLIB,
            E_TYPE_MODEL,
            E_TYPE_MATERIAL,
            E_TYPE_TEXTURE,
            E_TYPE_ARCHIVE,
            E_TYPE_FONT,
        };

        virtual ~Resource();

        virtual Type getType() const = 0;

        uint32_t getID() const
        {
            return mID;
        }

        uint32_t getCloneID() const
        {
            return mCloneID;
        }

        bool isCloned() const
        {
            return (mCloneID != 0);
        }

        size_t getSize() const
        {
            return mSize;
        }

        const String &getName() const
        {
            return mName;
        }

        bool isLoaded() const
        {
            return mIsLoaded;
        }

    protected:
        Resource(const String &name);

        virtual bool load() = 0;
        virtual void unload();
        virtual ResourcePtr clone() const = 0;

    protected:
        size_t      mSize;      /** size of this resource */
        int32_t     mID;        /** ID of this resource */
        int32_t     mCloneID;   /** If this resource is cloned from another, this value is none zero */
        bool        mIsLoaded;  /** loaded flag */
        String      mName;      /** name of the resource */
    };
}


#endif  /*__T3D_RESOURCE_H__*/
