
#include "Misc/T3DObject.h"


namespace Tiny3D
{
    Object::Object()
        : mReferCount(1)
        , mAutoReleaseCount(0)
    {

    }

    Object::~Object()
    {

    }

    Object *Object::acquire()
    {
        ++mReferCount;
        return this;
    }

    void Object::release()
    {
        if (--mReferCount == 0)
        {
            delete this;
        }
    }

    void Object::autorelease()
    {
        ObjectPoolManager::getInstance().addObject(this);
    }

    ////////////////////////////////////////////////////////////////////////////

    AutoreleasePool::AutoreleasePool()
        : mThreadID(std::this_thread::get_id())
    {
        
    }

    AutoreleasePool::~AutoreleasePool()
    {
        clear();
    }

    void AutoreleasePool::addObject(Object *object)
    {
        if (object != nullptr)
        {
            object->acquire();
            object->mAutoReleaseCount++;
            mObjects.push_back(object);
        }
    }

    void AutoreleasePool::removeObject(Object *object)
    {
        if (object != nullptr)
        {
            ObjectListItr itr = mObjects.begin();

            while (itr != mObjects.end())
            {
                if (object == *itr)
                {
                    mObjects.erase(itr);
                    object->mAutoReleaseCount--;
                    object->release();
                    break;
                }

                ++itr;
            }
        }
    }

    void AutoreleasePool::clear()
    {
        ObjectListItr itr = mObjects.begin();

        while (itr != mObjects.end())
        {
            Object *object = *itr;

            while (--object->mAutoReleaseCount)
            {
                object->release();
            }

            object->release();

            ++itr;
        }

        mObjects.clear();
    }

    ////////////////////////////////////////////////////////////////////////////

    ObjectPoolManager::ObjectPoolManager()
    {

    }

    ObjectPoolManager::~ObjectPoolManager()
    {

    }
    
    void ObjectPoolManager::addObjectPool(AutoreleasePool *pool)
    {
        AutoLock locker(mPoolsMutex);
        mPools.insert(AutoreleasePoolValue(pool->getThreadID(), pool));
    }

    void ObjectPoolManager::removeObjectPool(AutoreleasePool *pool)
    {
        AutoLock locker(mPoolsMutex);
        mPools.erase(pool->getThreadID());
    }

    void ObjectPoolManager::addObject(Object *object)
    {
        AutoLock locker(mPoolsMutex);
        AutoreleasePoolsItr itr = mPools.find(std::this_thread::get_id());

        if (itr != mPools.end())
        {
            AutoreleasePool *pool = itr->second;
            pool->addObject(object);
        }
    }

    void ObjectPoolManager::removeObject(Object *object)
    {
        AutoLock locker(mPoolsMutex);
        AutoreleasePoolsItr itr = mPools.find(std::this_thread::get_id());

        if (itr != mPools.end())
        {
            AutoreleasePool *pool = itr->second;
            pool->removeObject(object);
        }
    }

    void ObjectPoolManager::clear()
    {
        AutoLock locker(mPoolsMutex);
        AutoreleasePoolsItr itr = mPools.find(std::this_thread::get_id());

        if (itr != mPools.end())
        {
            AutoreleasePool *pool = itr->second;
            pool->clear();
        }
    }
}
