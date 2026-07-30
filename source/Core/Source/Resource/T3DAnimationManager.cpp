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


#include "Resource/T3DAnimationManager.h"
#include "Resource/T3DSkeletalAnimation.h"
#include "Serializer/T3DSerializerManager.h"


namespace Tiny3D
{   
    //--------------------------------------------------------------------------

    AnimationManagerPtr AnimationManager::create()
    {
        return T3D_NEW AnimationManager();
    }
    
    //--------------------------------------------------------------------------

    SkeletalAnimationPtr AnimationManager::createSkeletalAnimation(const String &name, const AnimationClips &clips, const UUID &uuid/* = UUID::INVALID*/)
    {
#if defined (T3D_EDITOR)
        // 编辑器/工具环境下允许指定 UUID（用于保留已有资源 guid），创建时即设好
        return smart_pointer_cast<SkeletalAnimation>(createResource(name, 2, &clips, &uuid));
#else
        return smart_pointer_cast<SkeletalAnimation>(createResource(name, 1, &clips));
#endif
    }

    //--------------------------------------------------------------------------

    SkeletalAnimationPtr AnimationManager::loadSkeletalAnimation(Archive *archive, const String &filename)
    {
        return smart_pointer_cast<SkeletalAnimation>(load(archive, filename));
    }

    //--------------------------------------------------------------------------

    SkeletalAnimationPtr AnimationManager::loadSkeletalAnimation(Archive *archive, const UUID &uuid)
    {
        return smart_pointer_cast<SkeletalAnimation>(load(archive, uuid));
    }

    //--------------------------------------------------------------------------

    TResult AnimationManager::saveSkeletalAnimation(Archive *archive, const String &filename, SkeletalAnimation *skeletalAni)
    {
        return save(archive, filename, skeletalAni);
    }

    //--------------------------------------------------------------------------

    TResult AnimationManager::saveSkeletalAnimation(Archive *archive, SkeletalAnimation *skeletalAni)
    {
        return save(archive, skeletalAni);
    }

    //--------------------------------------------------------------------------

    ResourcePtr AnimationManager::newResource(const String &name, int32_t argc, va_list args)
    {
        T3D_ASSERT(argc == 1 || argc == 2);
        AnimationClips *clips = va_arg(args, AnimationClips*);
        ResourcePtr anim = SkeletalAnimation::create(name, *clips);
#if defined (T3D_EDITOR)
        if (argc == 2)
        {
            const UUID *uuid = va_arg(args, const UUID*);
            applyCreationUUID(anim, uuid != nullptr ? *uuid : UUID::INVALID);
        }
#endif
        return anim;
    }

    //--------------------------------------------------------------------------

    ResourcePtr AnimationManager::loadResource(const String &name, DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<SkeletalAnimation>(stream);
    }

    //--------------------------------------------------------------------------

    ResourcePtr AnimationManager::loadResource(DataStream &stream)
    {
        return T3D_SERIALIZER_MGR.deserialize<SkeletalAnimation>(stream);
    }

    //--------------------------------------------------------------------------

    TResult AnimationManager::saveResource(DataStream &stream, Resource *res)
    {
        T3D_ASSERT(res->getType() == Resource::Type::kSkeletalAnimation);
        SkeletalAnimation *skeletalAni = static_cast<SkeletalAnimation*>(res);
        return T3D_SERIALIZER_MGR.serialize(stream, skeletalAni);
    }

    //--------------------------------------------------------------------------

    T3D_INSTANTIATE_SINGLETON(AnimationManager)
}

