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
        return new AnimationManager();
    }
    
    //--------------------------------------------------------------------------

    SkeletalAnimationPtr AnimationManager::createSkeletalAnimation(const String &name)
    {
        return smart_pointer_cast<SkeletalAnimation>(createResource(name, 0));
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

    TResult AnimationManager::saveSkeletalAnimation(Archive *archive, const String &filename, SkeletalAnimation *animation)
    {
        return save(archive, filename, animation);
    }

    //--------------------------------------------------------------------------

    TResult AnimationManager::saveSkeletalAnimation(Archive *archive, SkeletalAnimation *animation)
    {
        return save(archive, animation);
    }

    //--------------------------------------------------------------------------

    ResourcePtr AnimationManager::newResource(const String &name, int32_t argc, va_list args)
    {
        return SkeletalAnimation::create(name);
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
        T3D_ASSERT(res->getType() == Resource::Type::kMesh);
        SkeletalAnimation *skeletalAni = static_cast<SkeletalAnimation*>(res);
        return T3D_SERIALIZER_MGR.serialize(stream, skeletalAni);
    }

    //--------------------------------------------------------------------------
}

