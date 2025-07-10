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

#ifndef __T3D_SKELETAL_ANIMATION_H__
#define __T3D_SKELETAL_ANIMATION_H__


#include "Resource/T3DResource.h"


namespace Tiny3D
{
    TCLASS()
    class Keyframe : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        enum Type : uint32_t
        {
            Translation = 0,
            Rotation,
            Scaling
        };
        
        virtual Type getType() const = 0;

        TPROPERTY(RTTRFuncName="Time", RTTRFuncType="getter")
        uint32_t getTime() const { return mTime; }

    private:
        TPROPERTY(RTTRFuncName="Type", RTTRFuncType="setter")
        void setTime(uint32_t time) { mTime = time; }
        
    protected:
        /// 关键帧时间
        uint32_t mTime {0};
    };
    
    TCLASS()
    class KfTranslation : public Keyframe
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        Type getType() const override { return Type::Translation; }

        /// 关键帧位置
        TPROPERTY()
        Vector3 translation {0, 0, 0};
    };

    TSTRUCT()
    struct KfRotation
    {
        /// 关键帧时间
        TPROPERTY()
        uint32_t time {0};

        /// 关键帧旋转
        TPROPERTY()
        Quaternion rotation {};
    };

    TSTRUCT()
    struct KfScaling
    {
        /// 关键帧时间
        TPROPERTY()
        uint32_t time {0};

        /// 关键帧缩放
        TPROPERTY()
        Vector3 scaling {0, 0, 0};
    };
    
    using KfTranslations = TArray<KfTranslation>;
    using KfRotations = TArray<KfRotation>;
    using KfScalings = TArray<KfScaling>;
    
    TSTRUCT()
    struct BoneAnimation
    {
        /// 骨骼动画平移数据
        TPROPERTY()
        KfTranslations bonesTranslation {};

        /// 骨骼动画旋转数据
        TPROPERTY()
        KfRotations  bonesRotation {};

        /// 骨骼动画缩放数据
        TPROPERTY()
        KfScalings bonesScaling {};
    };

    using BoneAnimations = TMap<String, BoneAnimation>;

    /**
     * @brief 骨骼动画片段
     */
    TSTRUCT()
    struct AnimationClip
    {
        /// 动画片段名称
        TPROPERTY()
        String name {};

        /// 动画时长
        TPROPERTY()
        uint32_t  duration {0};

        /// 骨骼动画数据
        TPROPERTY()
        BoneAnimations boneAnimations {};
    };

    using AnimationClips = TMap<String, AnimationClip>;
    
    /**
     * \brief 骨骼动画数据资源
     */
    TCLASS()
    class T3D_ENGINE_API SkeletalAnimation : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 给加载 skeletal animation 资源时用来创建 SkeletalAnimation 对象
         * \param [in] name : skeletal animation 名称
         * \return 调用成功返回一个 skeletal animation 对象
         */
        static SkeletalAnimationPtr create(const String &name);

        static SkeletalAnimationPtr create(const String &name, const AnimationClips &clips);

        /**
         * \brief 析构函数
         */
        ~SkeletalAnimation() override;

        Type getType() const override;

        TPROPERTY(RTTRFuncName="AnimationClips", RTTRFuncType="getter")
        const AnimationClips &getAnimationClips() const { return mAnimationClips; }
        
    protected:
        SkeletalAnimation() = default;
        
        SkeletalAnimation(const String &name);

        SkeletalAnimation(const String &name, const AnimationClips &clips);
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        TResult onCreate() override;
        
        TResult onLoad(Archive *archive) override;
        
        TResult generateRenderResource(Archive *archive);

    private:
        TPROPERTY(RTTRFuncName="AnimationClips", RTTRFuncType="setter")
        void setAnimationClips(const AnimationClips &clips) { mAnimationClips = clips; }
        
    protected:
        /// 动画片段数据
        AnimationClips mAnimationClips {};
    };
}


#endif    /*__T3D_SKELETAL_ANIMATION_H__*/
