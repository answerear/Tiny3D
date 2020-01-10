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


#ifndef __T3D_RTTI_H__
#define __T3D_RTTI_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    /**
     * @class   RTTI
     * @brief   用于运行时动态类型检查机制 
     */
    class T3D_ENGINE_API RTTI
    {
        T3D_DISABLE_COPY(RTTI);

    public:
        static const size_t MAX_BASE_CLASS_COUNT = 8;
        static const size_t MAX_NAME_LENGTH = 64;

        /**
         * @fn  RTTI::RTTI();
         * @brief   构造函数
         */
        RTTI(const char *name);

        RTTI(const char *name, const RTTI *baseClass);

        RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1);

        RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1, 
            const RTTI *baseClass2);

        RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
            const RTTI *baseClass2, const RTTI *baseClass3);

        RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
            const RTTI *baseClass2, const RTTI *baseClass3,
            const RTTI *baseClass4);

        RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
            const RTTI *baseClass2, const RTTI *baseClass3,
            const RTTI *baseClass4, const RTTI *baseClass5);

        RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
            const RTTI *baseClass2, const RTTI *baseClass3,
            const RTTI *baseClass4, const RTTI *baseClass5,
            const RTTI *baseClass6);

        RTTI(const char *name, const RTTI *baseClass0, const RTTI *baseClass1,
            const RTTI *baseClass2, const RTTI *baseClass3,
            const RTTI *baseClass4, const RTTI *baseClass5,
            const RTTI *baseClass6, const RTTI *baseClass7);

        const char *getName() const
        {
            return mName;
        }

        size_t getBaseClassCount() const
        {
            return mBaseClassesCount;
        }

        const RTTI *getBaseClassRTTI(size_t idx) const
        {
            T3D_ASSERT(idx < mBaseClassesCount);
            return mBaseClasses[idx];
        }

        bool isSameAs(const RTTI *rtti) const
        {
            return (this == rtti);
        }

        bool isKindOf(const RTTI *rtti) const;

    private:
        const RTTI *mBaseClasses[MAX_BASE_CLASS_COUNT];
        const size_t mBaseClassesCount;
        char mName[MAX_NAME_LENGTH];
    };

    template <typename TYPE, typename OBJ>
    inline bool isSameAs(OBJ *obj)
    {
        return TYPE::getStaticRTTI()->isSameAs(obj->getRTTI());
    }

    template <typename TYPE, typename OBJ>
    inline bool isSameAs(const OBJ &obj)
    {
        return TYPE::getStaticRTTI()->isSameAs(obj.getRTTI());
    }

    template <typename TYPE, typename OBJ>
    inline bool isKindOf(OBJ *obj)
    {
        return TYPE::getStaticRTTI()->isKindOf(obj->getRTTI());
    }

    template <typename TYPE, typename OBJ>
    inline bool isKindOf(const OBJ &obj)
    {
        return TYPE::getStaticRTTI()->isKindOf(obj.getRTTI());
    }

    #define T3D_DECLARE_CLASS() \
        public: \
            static const RTTI *getStaticRTTI() { return &msRTTI; } \
            virtual const RTTI *getRTTI() const { return &msRTTI; } \
        private: \
            static const RTTI msRTTI;

    #define T3D_IMPLEMENT_CLASS_NO_BASECLASS(className) \
        const RTTI className::msRTTI(#className);

    #define T3D_IMPLEMENT_CLASS_BASECLASS_1(className, b0) \
        const RTTI className::msRTTI(#className, b0::getStaticRTTI());

    #define T3D_IMPLEMENT_CLASS_BASECLASS_2(className, b0, b1) \
        const RTTI className::msRTTI(#className, b0::getStaticRTTI(), \
            b1::getStaticRTTI());

    #define T3D_IMPLEMENT_CLASS_BASECLASS_3(className, b0, b1, b2) \
        const RTTI className::msRTTI(#className, b0::getStaticRTTI(), \
                b1::getStaticRTTI(), b2::getStaticRTTI());

    #define T3D_IMPLEMENT_CLASS_BASECLASS_4(className, b0, b1, b2, b3) \
        const RTTI className::msRTTI(#className, b0::getStaticRTTI(), \
                b1::getStaticRTTI(), b2::getStaticRTTI(), \
                b3::getStaticRTTI());

    #define T3D_IMPLEMENT_CLASS_BASECLASS_5(className, b0, b1, b2, b3, b4) \
        const RTTI className::msRTTI(#className, b0::getStaticRTTI(), \
                b1::getStaticRTTI(), b2::getStaticRTTI(), \
                b3::getStaticRTTI(), b4::getStaticRTTI());

    #define T3D_IMPLEMENT_CLASS_BASECLASS_6(className, b0, b1, b2, b3, b4, b5) \
            const RTTI className::msRTTI(#className, b0::getStaticRTTI(), \
                    b1::getStaticRTTI(), b2::getStaticRTTI(), \
                    b3::getStaticRTTI(), b4::getStaticRTTI(), \
                    b5::getStaticRTTI());

    #define T3D_IMPLEMENT_CLASS_BASECLASS_7(className, b0, b1, b2, b3, b4, b5, b6) \
            const RTTI className::msRTTI(#className, b0::getStaticRTTI(), \
                    b1::getStaticRTTI(), b2::getStaticRTTI(), \
                    b3::getStaticRTTI(), b4::getStaticRTTI(), \
                    b5::getStaticRTTI(), b6::getStaticRTTI());

    #define T3D_IMPLEMENT_CLASS_BASECLASS_8(className, b0, b1, b2, b3, b4, b5, b6, b7) \
            const RTTI className::msRTTI(#className, b0::getStaticRTTI(), \
                    b1::getStaticRTTI(), b2::getStaticRTTI(), \
                    b3::getStaticRTTI(), b4::getStaticRTTI(), \
                    b5::getStaticRTTI(), b6::getStaticRTTI(), \
                    b7::getStaticRTTI());
}


#endif  /*__T3D_RTTI_H__*/
