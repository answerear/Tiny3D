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


#include "Resource/T3DSkeletonManager.h"
#include "Resource/T3DSkeleton.h"
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{   
    //--------------------------------------------------------------------------

    SkeletonManagerPtr SkeletonManager::create()
    {
        return T3D_NEW SkeletonManager();
    }
    
    //--------------------------------------------------------------------------

    SkeletonPtr SkeletonManager::createSkeleton(const String &name, GameObject *rootBoneGameObject, const UUID &uuid/* = UUID::INVALID*/)
    {
#if defined (T3D_EDITOR)
        // 编辑器/工具环境下允许指定 UUID（用于保留已有资源 guid），创建时即设好
        return smart_pointer_cast<Skeleton>(createResource(name, 2, rootBoneGameObject, &uuid));
#else
        return smart_pointer_cast<Skeleton>(createResource(name, 1, rootBoneGameObject));
#endif
    }

    //--------------------------------------------------------------------------

    SkeletonPtr SkeletonManager::loadSkeleton(Archive *archive, const String &filename)
    {
        return smart_pointer_cast<Skeleton>(load(archive, filename));
    }

    //--------------------------------------------------------------------------

    SkeletonPtr SkeletonManager::loadSkeleton(Archive *archive, const UUID &uuid)
    {
        return smart_pointer_cast<Skeleton>(load(archive, uuid));
    }

    //--------------------------------------------------------------------------

    TResult SkeletonManager::saveSkeleton(Archive *archive, const String &filename, Skeleton *skeleton)
    {
        return save(archive, filename, skeleton);
    }

    //--------------------------------------------------------------------------

    TResult SkeletonManager::saveSkeleton(Archive *archive, Skeleton *skeleton)
    {
        return save(archive, skeleton);
    }

    //--------------------------------------------------------------------------

    ResourcePtr SkeletonManager::newResource(const String &name, int32_t argc, va_list args)
    {
        T3D_ASSERT(argc == 1 || argc == 2);
        GameObject *rootBoneGameObject = va_arg(args, GameObject*);
        ResourcePtr skeleton = Skeleton::create(name, rootBoneGameObject);
#if defined (T3D_EDITOR)
        if (argc == 2)
        {
            const UUID *uuid = va_arg(args, const UUID*);
            applyCreationUUID(skeleton, uuid != nullptr ? *uuid : UUID::INVALID);
        }
#endif
        return skeleton;
    }

    //--------------------------------------------------------------------------

    ResourcePtr SkeletonManager::loadResource(const String &name, DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<Skeleton>(stream);
    }

    //--------------------------------------------------------------------------

    ResourcePtr SkeletonManager::loadResource(DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<Skeleton>(stream);
    }

    //--------------------------------------------------------------------------

    TResult SkeletonManager::saveResource(DataStream &stream, Resource *res)
    {
        T3D_ASSERT(res->getType() == Resource::Type::kSkeleton);
        Skeleton *skeleton = static_cast<Skeleton*>(res);
        return T3D_SERIALIZER_MGR.serialize(stream, skeleton);
    }

    //--------------------------------------------------------------------------

    T3D_INSTANTIATE_SINGLETON(SkeletonManager)
}

