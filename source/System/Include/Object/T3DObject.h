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


#ifndef __T3D_OBJECT_H__
#define __T3D_OBJECT_H__


#include "T3DSystemPrerequisites.h"
#include "T3DSystem.h"


namespace Tiny3D
{

    #define T3D_TRACE_OBJECT    0
    
    /**
     * @class   Object
     * @brief   引擎所有对象基类
     */
    TCLASS()
    class T3D_SYSTEM_API Object
    {
        TRTTI_ENABLE()

        template<typename T>
        friend class SmartPtr;

        template<typename T>
        friend class ThreadSafePtr;
        
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
         * @fn  uint32_t Object::referCount() const
         * @brief   返回对象当前引用计数
         * @return  An uint32_t.
         */
        uint32_t referCount() const
        {
            return mReferCount;
        }

    protected:
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

        void enableThreadSafe(bool enable);

        void printDebugInfo();

        virtual bool onPreSave();

        virtual bool onPostSave();
        
        virtual bool onPostLoad();

    private:
        /// 引用计数
        uint32_t    mReferCount {0};
        /// 线程安全对象引用计数
        uint32_t    mThreadSafeRefCount {0};
        /// 用于多线程的同步锁
        ISyncObject *mSyncObject {nullptr};
        /// 是否开启线程安全
        bool    mIsThreadSafe {false};

#if T3D_TRACE_OBJECT
        String  mClsName;
        bool    mHasOutputDebug {false};
#endif
    };
}

#if 0
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
    const T TemplateMax(const T &a, const T b)
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
        void Insert(const T &data, bool check)
        {
            Insert(data);
        }

        TFUNCTION()
        void Test(const T &data)
        {
            
        }

        TPROPERTY()
        T InitData;
        
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

    TCLASS()
    class IIntersect
    {
        TRTTI_ENABLE()
        TRTTI_FRIEND
        
    public:
        
        virtual ~IIntersect() {}

        TFUNCTION()
        virtual bool test() = 0;

    protected:
        IIntersect() {}
    };

    TCLASS()
    class SphereIntersect : public IIntersect
    {
        TRTTI_ENABLE(IIntersect)
        TRTTI_FRIEND

    public:
        SphereIntersect() {}
        virtual ~SphereIntersect() {}

        TFUNCTION()
        bool test() override
        {
            return true;
        }
    };
    
    TCLASS("EditorOnly", "Category"="Derived")
    class Derived : public Tiny3D::Object
    {
        TRTTI_ENABLE(Tiny3D::Object);
        
    public:
        TFUNCTION(RTTRAsConstructor)
        static Derived *create()
        {
            return new Derived();
        }

        TPROPERTY(Name="value", Type="getter", "Description"="This is the value.")
        const std::string &getValue() const
        {
            return mStr;
        }

    protected:
        std::string mStr;

        Derived() {}
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

            TFUNCTION()
            TemplateArray<int, 3> GetArray() const
            {
                return data;
            }

            TFUNCTION()
            void SetArray(const TemplateArray<int, 3> &arr)
            {
                data = arr;
            }

        protected:
            TemplateArray<int, 3> data;
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
                b = TemplateMax(a, b);
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
#endif

#endif  /*__T3D_OBJECT_H__*/
