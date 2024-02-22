/*******************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Answer Wong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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
