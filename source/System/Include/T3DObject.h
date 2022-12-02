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
    TCLASS()
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


TFUNCTION("Description"="This is a function.")
inline void Globalfunction(float fval)
{
}

TFUNCTION("Description"="This is a function.")
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
    TFUNCTION()
    template<typename T>
    T TemplateMax(const T &a, const T &b)
    {
        return (a > b ? a : b);
    }

    // template<typename T>
    // using TemplateMaxAlias = T(*)(const T&, const T &);
    
    // TemplateMaxAlias<int> TemplateMaxInt;
    
    // using TemplateMaxInt = int(*TemplateMax<int>)(const int&, const in&);
    
    // typedef (int)(*TemplateMaxInt)(const int&, const int&)
    // using TemplateMaxInt = int (*)(const int&, const int &);

    TCLASS()
    template<typename T, template<typename U> class Container>
    class XCls
    {
        TRTTI_ENABLE();
        
    protected:
        Container<T> c;
    };

    TCLASS()
    template<typename K, typename V>
    class TemplateMap
    {
        TRTTI_ENABLE();

    public:
        TemplateMap() = default;

        virtual ~TemplateMap() = default;
    };

    template<size_t N, typename T>
    class TemplateList
    {
        
    };

    TCLASS()
    template<typename T, size_t N>
    class TemplateArray
    {
        TRTTI_ENABLE();
        
    public:
        TemplateArray()
            : mData(nullptr)
        {}

        virtual ~TemplateArray()
        {
            delete mData;
            mData = nullptr;
        }

        TFUNCTION()
        void Insert(const T &data)
        {
            if (mSize >= mCapacity)
            {
                Extend();
            }
            else if (mSize <= mCapacity / 4)
            {
                Shrink();
            }

            size_t index = mSize;
            mData[index] = data;
        }

        TFUNCTION()
        void Test(const T &data)
        {
            
        }
        
    protected:
        void Extend()
        {
            
        }
        
        void Shrink()
        {
            
        }
        
        T *mData;
        size_t mSize;
        size_t mCapacity;
    };
    
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
    
    TCLASS("EditorOnly", "Category"="Derived")
    class Derived : public Tiny3D::Object
    {
        TRTTI_ENABLE(Tiny3D::Object);
        
    public:
        Derived() {}

        TPROPERTY(Name="value", Type="getter", "Description"="This is the value.")
        const std::string &getValue() const
        {
            return mStr;
        }

        std::string mStr;
    };

    TCLASS()
    class Base
    {
        TRTTI_ENABLE();
    public:
        Base() = default;

        TPROPERTY()
        TemplateArray<float, 10> items;

        TPROPERTY()
        TemplateMap<int, float> caches;
        // TPROPERTY()
        // XCls<int, std::vector> cls;
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
            TRTTI_ENABLE(Derived);
        public:
            Object() = default;
        };

        TCLASS()
        class Derived : public Object
        {
            TRTTI_ENABLE(Test::AA::Object);
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
                TRTTI_ENABLE();
                
            public:
                NestedDerived() = default;
            };

            template<typename T>
            Derived(T val)
            {}

            TFUNCTION()
            template<typename T>
            T TemplateFunc(T a)
            {
                T t = {};
                t = a;
                return t;
            }

            TFUNCTION()
            int32_t XXfunction(int a, float c)
            {
                int b = 0;
                int r = TemplateMax<int>(a, b);
                float t0 = 1.0f, t1 = 2.0f;
                TemplateMax(t0, t1);
                TemplateMap<int, int> map;
                TemplateArray<int, 5> arr;
                return r;
            }
            
            TFUNCTION("Description"="This is a static function.")
            static void XXfunction()
            {
                
            }
			
			TFUNCTION("Description"="This is a member function.")
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
            
            // TPROPERTY("Description"="This is an integer value.")
            // int32_t iValue;
            //
            // TPROPERTY()
            // float32_t fValue;
            //
            // TPROPERTY()
            // char *szName;
            //
            // TPROPERTY()
            // EType eType;
        };
    }
}

namespace AA
{
    TCLASS()
    class Derived : public Test::Base
    {
        TRTTI_ENABLE(Test::Base);
        
    public:
        Derived() = default;
    };

    TCLASS()
    class AADerived : public Test::AA::Derived
    {
        TRTTI_ENABLE(Test::AA::Derived);
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
        TRTTI_ENABLE();
    public:
        Base1() = default;
    };

    TCLASS()
    class Base2
    {
        TRTTI_ENABLE();
    public:
        Base2() = default;
    };

    TCLASS()
    class MultiDerived : public Base1, public Base2
    {
        TRTTI_ENABLE(AA::Base1, AA::Base2);
    public:
        MultiDerived() = default;
    };
}

#endif  /*__T3D_OBJECT_H__*/
