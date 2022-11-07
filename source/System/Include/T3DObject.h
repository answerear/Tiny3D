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


#ifndef __T3D_OBJECT_H__
#define __T3D_OBJECT_H__


#include "T3DSystemPrerequisites.h"
#include "T3DClass.h"
#include "T3DSystem.h"


namespace Tiny3D
{
    /**
     * @class   Object
     * @brief   引擎所有对象基类
     */
    TCLASS(EditorOnly, Catogary=Base)
    class T3D_SYSTEM_API Object
    {
        T3D_DECLARE_CLASS();
        TRTTI_ENABLE();

    public:
        /**
         * @fn  Object::Object();
         * @brief   构造函数
         */
        Object();

        /**
         * @fn  virtual Object::~Object();
         * @brief   析构函数
         */
        virtual ~Object();

        /**
         * @fn  Object Object::*acquire();
         * @brief   持有对象，让对象引用计数加一
         * @return  Null if it fails, else a pointer to an Object.
         */
        Object *acquire();

        /**
         * @fn  void Object::release();
         * @brief   释放对象，让对象引用计数减一
         */
        void release();

        /**
         * @fn  uint32_t Object::referCount() const
         * @brief   返回对象当前引用计数
         * @return  An uint32_t.
         */
        uint32_t referCount() const
        {
            return mReferCount;
        }

    private:
        uint32_t    mReferCount;
    };
}

TFUNCTION()
inline void Globalfunction(float fval)
{}

TFUNCTION()
inline float Globalfunction()
{ return 0.0f; }

TENUM()
enum class AAType : uint32_t
{
    kNone = 0,
    kReal,
    kFake
};

namespace Test
{
    // TFUNCTION()
    // int NSFunction()
    // {
    //     return 0;
    // }

    TENUM()
    enum class AAType : uint32_t
    {
        kNone = 0,
        kReal,
        kFake
    };
    
    TCLASS()
    class Derived : public Tiny3D::Object
    {
    public:
        Derived() {}

        TPROPERTY(Name=value, Type=getter)
        const std::string &getValue() const
        {
            return mStr;
        }

        std::string mStr;
    };

    TCLASS()
    class Base
    {
    public:
        Base() = default;
    };

    // TFUNCTION()
    // void XXfunction()
    // {}
    //
    // TFUNCTION()
    // int XXfunction(int a)
    // { return 0; }

    namespace AA
    {
        TCLASS()
        class Object : public Derived
        {
        public:
            Object() = default;
        };

        TCLASS()
        class Derived : public Object
        {
        public:
			TENUM()
			enum class EType : uint32_t
			{
				kNull = 0,
				kBool,
				kChar,
				kInt8,
				kUInt8,
				kInt16,
				kUInt16,
				kInt32,
				kUInt32,
				kInt64,
				kUInt64,
				kSingle,
				kDouble,
				kPointer
			};
			
            Derived() = default;
            Derived(int32_t a)
            {}

            Derived(const Derived &other)
            {}

            Derived(Derived &&other)
            {}
            
            ~Derived()
            {}

            TCLASS()
            class NestedDerived
            {
            public:
                NestedDerived() = default;
            };

            TFUNCTION()
            int32_t XXfunction(int a, float c)
            {
                return 0;
            }

            TFUNCTION()
            static void XXfunction()
            {
                
            }
			
			TFUNCTION()
			int32_t TestFunction() const
			{
				return 0;
			}

            TFUNCTION()
            void XXfunction(int a, Object *ob, float c)
            {
                TCLASS()
                class TempDerived
                {
                public:
                    TempDerived() = default;

                    void test();
                };
                TempDerived temp;
            }
			
			TPROPERTY()
			int32_t iValue;
			
			TPROPERTY()
			float32_t fValue;
			
			TPROPERTY()
			char *szName;

            TPROPERTY()
            EType eType;
        };
    }
}

namespace AA
{
    TCLASS()
    class Derived : public Test::Base
    {
    public:
        Derived() = default;
    };

    TCLASS()
    class AADerived : public Test::AA::Derived
    {
    public:
        AADerived() = default;
    };

    using namespace Tiny3D;
    
    TCLASS()
    class TestManager : public Singleton<TestManager>
    {
    public:
        
    };

    TCLASS()
    class Base1
    {
    public:
        Base1() = default;
    };

    TCLASS()
    class Base2
    {
    public:
        Base2() = default;
    };

    TCLASS()
    class MultiDerived : public Base1, public Base2
    {
    public:
        MultiDerived() = default;
    };
}

#endif  /*__T3D_OBJECT_H__*/
