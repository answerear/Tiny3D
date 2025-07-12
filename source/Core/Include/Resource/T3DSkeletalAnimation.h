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
    class T3D_ENGINE_API Keyframe : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        enum class Type : uint32_t
        {
            Translation = 0,
            Orientation,
            Scaling
        };
        
        virtual Type getType() const = 0;

        TPROPERTY(RTTRFuncName="Time", RTTRFuncType="getter")
        uint32_t getTime() const { return mTime; }

    private:
        TPROPERTY(RTTRFuncName="Type", RTTRFuncType="setter")
        void setTime(uint32_t time) { mTime = time; }
        
    protected:
        Keyframe() = default;

        Keyframe(uint32_t time);
        
        /// 关键帧时间
        uint32_t mTime {0};
    };
    
    TCLASS()
    class T3D_ENGINE_API KfTranslation : public Keyframe
    {
        TRTTI_ENABLE(Keyframe)
        TRTTI_FRIEND

    public:
        static KfTranslationPtr create(uint32_t time, const Vector3 &pos);
        
        Type getType() const override { return Type::Translation; }

        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="getter")
        const Vector3 &getTranslation() const { return mTranslation; }
        
    private:
        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="setter")
        void setTranslation(const Vector3 &pos) { mTranslation = pos; }
        
    protected:
        KfTranslation() = default;

        KfTranslation(uint32_t time, const Vector3 &translation);
        
        /// 关键帧位移
        Vector3 mTranslation {0, 0, 0};
    };

    TCLASS()
    class T3D_ENGINE_API KfOrientation : public Keyframe
    {
        TRTTI_ENABLE(Keyframe)
        TRTTI_FRIEND

    public:
        static KfOrientationPtr create(uint32_t time, const Quaternion &orientation);

        Type getType() const override { return Type::Orientation; }

        TPROPERTY(RTTRFuncName="Orientation", RTTRFuncType="getter")
        const Quaternion &getOrientation() const { return mOrientation; }

    private:
        TPROPERTY(RTTRFuncName="Orientation", RTTRFuncType="setter")
        void setOrientation(const Quaternion &orientation) { mOrientation = orientation; }

    protected:
        KfOrientation() = default;

        KfOrientation(uint32_t time, const Quaternion &orientation);
        
        /// 关键帧旋转
        Quaternion mOrientation {1.0f, 0.0f, 0.0f, 0.0f};
    };

    TCLASS()
    class T3D_ENGINE_API KfScaling : public Keyframe
    {
        TRTTI_ENABLE(Keyframe)
        TRTTI_FRIEND

    public:
        static KfScalingPtr create(uint32_t time, const Vector3 &scaling);

        Type getType() const override { return Type::Scaling; }

        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="getter")
        const Vector3 &getScaling() const { return mScaling; }

    private:
        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="setter")
        void setScaling(const Vector3 &scaling) { mScaling = scaling; }
        
    protected:
        KfScaling() = default;

        KfScaling(uint32_t time, const Vector3 &scaling);
        
        /// 关键帧缩放
        Vector3 mScaling {1.0f, 1.0f, 1.0f};
    };

    /**
     * @brief 骨骼动画片段
     */
    TCLASS()
    class T3D_ENGINE_API AnimationClip : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        static AnimationClipPtr create(const String &name, uint32_t duration,
            const TranslationTrack &translationTrack, const OrientationTrack &orientationTrack,
            const ScalingTrack &scalingTrack);

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        TPROPERTY(RTTRFuncName="Duration", RTTRFuncType="getter")
        uint32_t getDuration() const { return mDuration; }

        TPROPERTY(RTTRFuncName="TranslationTrack", RTTRFuncType="getter")
        const TranslationTrack &getTranslationTrack() const { return mTranslationTrack; }

        TPROPERTY(RTTRFuncName="OrientationTrack", RTTRFuncType="getter")
        const OrientationTrack &getOrientationTrack() const { return mOrientationTrack; }

        TPROPERTY(RTTRFuncName="ScalingTrack", RTTRFuncType="getter")
        const ScalingTrack &getScalingTrack() const { return mScalingTrack; }

    private:
        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="setter")
        void setName(const String &name) { mName = name; }

        TPROPERTY(RTTRFuncName="Duration", RTTRFuncType="setter")
        void setDuration(uint32_t duration) { mDuration = duration; }

        TPROPERTY(RTTRFuncName="TranslationTrack", RTTRFuncType="setter")
        void setTranslationTrack(const TranslationTrack &track) { mTranslationTrack = track; }

        TPROPERTY(RTTRFuncName="OrientationTrack", RTTRFuncType="setter")
        void setOrientationTrack(const OrientationTrack &track) { mOrientationTrack = track; }

        TPROPERTY(RTTRFuncName="ScalingTrack", RTTRFuncType="setter")
        void setScalingTrack(const ScalingTrack &track) { mScalingTrack = track; }
        
    protected:
        AnimationClip() = default;

        AnimationClip(const String &name, uint32_t duration,
            const TranslationTrack &translationTrack, const OrientationTrack &orientationTrack,
            const ScalingTrack &scalingTrack);
        
        /// 动画片段名称
        String mName {};

        /// 动画时长
        uint32_t mDuration {0};

        /// 骨骼动画平移数据
        TranslationTrack mTranslationTrack {};

        /// 骨骼动画旋转数据
        OrientationTrack  mOrientationTrack {};

        /// 骨骼动画缩放数据
        ScalingTrack mScalingTrack {};
    };
    
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
