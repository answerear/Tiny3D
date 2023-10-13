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


#ifndef __T3D_ASSIGNABLE_OBJECT_MANAGER_H__
#define __T3D_ASSIGNABLE_OBJECT_MANAGER_H__


#include "T3DSystemPrerequisites.h"
#include "T3DSystem.h"


namespace Tiny3D
{
    class AssignableObjectManager;
    T3D_DECLARE_SMART_PTR(AssignableObjectManager);

    class T3D_SYSTEM_API AssignableObjectManager 
        : public Object
        , public Singleton<AssignableObjectManager>
    {
    public:
        static AssignableObjectManagerPtr create();
        
        template<typename T>
        void setObject(const String &name, T *src, T dst)
        {
            AssignableObjectPtr obj = new AssignableObject<T>(src, dst);
            mAssignableObjects[name] = obj;
        }

        void assign()
        {
            for (auto item : mAssignableObjects)
            {
                item.second->assign();
            }
        }
        
    protected:
        class AssignableObject : public Object
        {
        public:
            virtual void assign() = 0;
        };

        template<typename T>
        class AssignableObjectT : public AssignableObject
        {
        public:        
            AssignableObjectT(T *srcValue, T dstValue)
                : mSrcValue(srcValue)
                , mDstValue(dstValue)
            {
            
            }

            void assign() override
            {
                *mSrcValue = mDstValue;
            }
        
        protected:
            T   *mSrcValue {nullptr};
            T   mDstValue {};
        };
        
        using AssignableObjectPtr = SmartPtr<AssignableObject>;
        using AssignableObjects = TMap<String, AssignableObjectPtr>;

        AssignableObjects   mAssignableObjects{};
    };
}


#endif  /*__T3D_ASSIGNABLE_OBJECT_MANAGER_H__*/
