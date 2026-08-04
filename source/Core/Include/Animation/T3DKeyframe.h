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

#ifndef __T3D_KEYFRAME_H__
#define __T3D_KEYFRAME_H__


#include "T3DPrerequisites.h"
#include "T3DTypedef.h"


namespace Tiny3D
{
    /**
     * \brief 骨骼动画关键帧抽象基类，携带时间戳与类型标识
     */
    TCLASS()
    class T3D_ENGINE_API Keyframe : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND
        
    public:
        /**
         * \brief 关键帧所携带的变换通道类型
         */
        enum class Type : uint32_t
        {
            Translation = 0,    ///< 平移
            Orientation,        ///< 旋转
            Scaling             ///< 缩放
        };
        
        /**
         * \brief 获取关键帧类型
         * \return 平移、旋转或缩放类型标识
         */
        virtual Type getType() const = 0;

        /// 获取关键帧时间戳（毫秒）
        TPROPERTY(RTTRFuncName="Time", RTTRFuncType="getter")
        uint32_t getTime() const { return mTime; }

    private:
        /// 设置关键帧时间戳（毫秒），供 RTTR 反序列化使用
        TPROPERTY(RTTRFuncName="Time", RTTRFuncType="setter")
        void setTime(uint32_t time) { mTime = time; }
        
    protected:
        /// 默认构造，时间戳为 0
        Keyframe() = default;

        /**
         * \brief 构造并设置时间戳
         * \param [in] time : 关键帧时间（毫秒）
         */
        Keyframe(uint32_t time);
        
        /// 关键帧时间戳（毫秒）
        uint32_t mTime {0};
    };
    
    /**
     * \brief 平移关键帧，存储某一时刻的 Vector3 位移
     */
    TCLASS()
    class T3D_ENGINE_API KfTranslation : public Keyframe
    {
        TRTTI_ENABLE(Keyframe)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建平移关键帧
         * \param [in] time : 关键帧时间（毫秒）
         * \param [in] pos : 平移向量
         * \return 新建的 KfTranslation 智能指针
         */
        static KfTranslationPtr create(uint32_t time, const Vector3 &pos);
        
        /// 返回 Type::Translation
        Type getType() const override { return Type::Translation; }

        /// 获取平移向量
        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="getter")
        const Vector3 &getTranslation() const { return mTranslation; }
        
    private:
        /// 设置平移向量，供 RTTR 反序列化使用
        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="setter")
        void setTranslation(const Vector3 &pos) { mTranslation = pos; }
        
    protected:
        KfTranslation() = default;

        /**
         * \brief 构造平移关键帧
         * \param [in] time : 关键帧时间（毫秒）
         * \param [in] translation : 平移向量
         */
        KfTranslation(uint32_t time, const Vector3 &translation);
        
        /// 平移值
        Vector3 mTranslation {0, 0, 0};
    };

    /**
     * \brief 旋转关键帧，存储某一时刻的 Quaternion 朝向
     */
    TCLASS()
    class T3D_ENGINE_API KfOrientation : public Keyframe
    {
        TRTTI_ENABLE(Keyframe)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建旋转关键帧
         * \param [in] time : 关键帧时间（毫秒）
         * \param [in] orientation : 旋转四元数
         * \return 新建的 KfOrientation 智能指针
         */
        static KfOrientationPtr create(uint32_t time, const Quaternion &orientation);

        /// 返回 Type::Orientation
        Type getType() const override { return Type::Orientation; }

        /// 获取旋转四元数
        TPROPERTY(RTTRFuncName="Orientation", RTTRFuncType="getter")
        const Quaternion &getOrientation() const { return mOrientation; }

    private:
        /// 设置旋转四元数，供 RTTR 反序列化使用
        TPROPERTY(RTTRFuncName="Orientation", RTTRFuncType="setter")
        void setOrientation(const Quaternion &orientation) { mOrientation = orientation; }

    protected:
        KfOrientation() = default;

        /**
         * \brief 构造旋转关键帧
         * \param [in] time : 关键帧时间（毫秒）
         * \param [in] orientation : 旋转四元数
         */
        KfOrientation(uint32_t time, const Quaternion &orientation);
        
        /// 旋转值，默认单位四元数
        Quaternion mOrientation {1.0f, 0.0f, 0.0f, 0.0f};
    };

    /**
     * \brief 缩放关键帧，存储某一时刻的 Vector3 缩放
     */
    TCLASS()
    class T3D_ENGINE_API KfScaling : public Keyframe
    {
        TRTTI_ENABLE(Keyframe)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建缩放关键帧
         * \param [in] time : 关键帧时间（毫秒）
         * \param [in] scaling : 缩放向量
         * \return 新建的 KfScaling 智能指针
         */
        static KfScalingPtr create(uint32_t time, const Vector3 &scaling);

        /// 返回 Type::Scaling
        Type getType() const override { return Type::Scaling; }

        /// 获取缩放向量
        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="getter")
        const Vector3 &getScaling() const { return mScaling; }

    private:
        /// 设置缩放向量，供 RTTR 反序列化使用
        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="setter")
        void setScaling(const Vector3 &scaling) { mScaling = scaling; }
        
    protected:
        KfScaling() = default;

        /**
         * \brief 构造缩放关键帧
         * \param [in] time : 关键帧时间（毫秒）
         * \param [in] scaling : 缩放向量
         */
        KfScaling(uint32_t time, const Vector3 &scaling);
        
        /// 缩放值，默认 (1, 1, 1)
        Vector3 mScaling {1.0f, 1.0f, 1.0f};
    };

}


#endif    /*__T3D_KEYFRAME_H__*/
