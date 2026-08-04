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
    /**
     * \brief 骨骼动画数据资源，存储 AnimationClip 集合
     */
    TCLASS()
    class T3D_ENGINE_API SkeletalAnimation : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 创建空白骨骼动画资源
         * \param [in] name : 动画资源名称
         * \return 新 SkeletalAnimation 智能指针
         */
        static SkeletalAnimationPtr create(const String &name);

        /**
         * \brief 创建并初始化 AnimationClips 的骨骼动画资源
         * \param [in] name : 动画资源名称
         * \param [in] clips : 动画片段集合
         * \return 新 SkeletalAnimation 智能指针
         */
        static SkeletalAnimationPtr create(const String &name, const AnimationClips &clips);

        /// 析构 SkeletalAnimation
        ~SkeletalAnimation() override;

        /**
         * \brief 返回资源类型标识
         * \return Type::kSkeletalAnimation
         */
        Type getType() const override;

        /// 获取动画片段集合
        TPROPERTY(RTTRFuncName="AnimationClips", RTTRFuncType="getter")
        const AnimationClips &getAnimationClips() const { return mAnimationClips; }
        
    protected:
        /// 默认构造
        SkeletalAnimation() = default;
        
        /**
         * \brief 构造骨骼动画资源
         * \param [in] name : 动画资源名称
         */
        SkeletalAnimation(const String &name);

        /**
         * \brief 构造并复制 AnimationClips
         * \param [in] name : 动画资源名称
         * \param [in] clips : 动画片段集合
         */
        SkeletalAnimation(const String &name, const AnimationClips &clips);
        
        /**
         * \brief 克隆骨骼动画（当前 cloneProperties 为空，不复制 clips）
         * \return 新 SkeletalAnimation 智能指针
         */
        ResourcePtr clone() const override;

        /**
         * \brief 克隆属性（当前实现为空，不复制 mAnimationClips）
         * \param [in] src : 源 SkeletalAnimation 资源
         */
        void cloneProperties(const Resource * const src) override;

        /**
         * \brief 资源创建回调，委托基类 onCreate
         * \return 基类 onCreate 返回值
         */
        TResult onCreate() override;
        
        /**
         * \brief 从 Archive 加载后调用 generateRenderResource
         * \param [in] archive : 资源档案
         * \return 加载或 generateRenderResource 失败返回对应错误码
         */
        TResult onLoad(Archive *archive) override;
        
        /**
         * \brief 加载后生成渲染相关资源（当前实现为空，恒返回 T3D_OK）
         * \param [in] archive : 资源档案
         * \return T3D_OK
         */
        TResult generateRenderResource(Archive *archive);

    private:
        /// RTTR：设置动画片段集合
        TPROPERTY(RTTRFuncName="AnimationClips", RTTRFuncType="setter")
        void setAnimationClips(const AnimationClips &clips) { mAnimationClips = clips; }
        
    protected:
        /// 动画片段数据
        AnimationClips mAnimationClips {};
    };
}


#endif    /*__T3D_SKELETAL_ANIMATION_H__*/
