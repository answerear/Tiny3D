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

}


#endif    /*__T3D_KEYFRAME_H__*/
