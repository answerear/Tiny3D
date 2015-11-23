

#ifndef __T3D_OBJECT_H__
#define __T3D_OBJECT_H__


#include "T3DPrerequisites.h"


namespace Tiny3D
{
    class T3D_ENGINE_API Object
    {
        friend class AutoreleasePool;

    public:
        Object();
        virtual ~Object();

        virtual Object *acquire();
        virtual void release();

        void autorelease();
        
        uint32_t referCount() const
        {
            return mReferCount;
        }

    private:
        uint32_t    mReferCount;
        uint32_t    mAutoReleaseCount;
    };

    class T3D_ENGINE_API AutoreleasePool
    {
    public:
        AutoreleasePool();
        ~AutoreleasePool();

        void addObject(Object *object);
        void removeObject(Object *object);

        void clear();

        std::thread::id getThreadID() { return mThreadID; }

    protected:
        typedef std::list<Object*>          ObjectList;
        typedef ObjectList::iterator        ObjectListItr;
        typedef ObjectList::const_iterator  ObjectListConstItr;

        ObjectList      mObjects;
        std::thread::id mThreadID;
    };

    class T3D_ENGINE_API ObjectPoolManager : public Singleton<ObjectPoolManager>
    {
    public:
        ObjectPoolManager();
        virtual ~ObjectPoolManager();

        void addObjectPool(AutoreleasePool *pool);
        void removeObjectPool(AutoreleasePool *pool);

        void addObject(Object *object);
        void removeObject(Object *object);

        void clear();

    private:
        typedef std::map<std::thread::id, AutoreleasePool*>     AutoreleasePools;
        typedef AutoreleasePools::iterator                      AutoreleasePoolsItr;
        typedef AutoreleasePools::const_iterator                AutoreleasePoolsConstItr;
        typedef AutoreleasePools::value_type                    AutoreleasePoolValue;

        typedef std::recursive_mutex    Mutex;
        typedef std::unique_lock<Mutex> AutoLock;

        AutoreleasePools        mPools;

        Mutex                   mPoolsMutex;
    };
}


#endif  /*__T3D_OBJECT_H__*/
