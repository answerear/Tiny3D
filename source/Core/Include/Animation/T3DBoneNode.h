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

#ifndef __T3D_BONE_NODE_H__
#define __T3D_BONE_NODE_H__


#include "Resource/T3DResourceManager.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API BoneNode : public Object
    {
        TRTTI_ENABLE(Object)
        TRTTI_FRIEND

    public:
        static const uint16_t kInvalidIndex;
        
        static BoneNodePtr create(const String &name, uint16_t parentIndex,
            const Vector3 &translation, const Quaternion &rotation,
            const Vector3 &scaling, const Matrix4 &offsetMatrix);

        /**
         * \brief 析构函数
         */
        ~BoneNode() override = default;

        TPROPERTY(RTTRFuncName="Name", RTTRFuncType="getter")
        const String &getName() const { return mName; }

        TPROPERTY(RTTRFuncName="ParentIndex", RTTRFuncType="getter")
        uint16_t getParentIndex() const
        {
            return mParentIndex;
        }

        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="getter")
        const Vector3 &getTranslation() const
        {
            return mTranslation;
        }

        TPROPERTY(RTTRFuncName="Rotation", RTTRFuncType="getter")
        const Quaternion &getRotation() const
        {
            return mRotation;
        }

        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="getter")
        const Vector3 &getScaling() const
        {
            return mScaling;
        }

        TPROPERTY(RTTRFuncName="OffsetMatrix", RTTRFuncType="getter")
        const Matrix4 &getOffsetMatrix() const
        {
            return mOffsetMatrix;
        }

    protected:
        BoneNode() = default;
        
        BoneNode(const String &name, uint16_t parentIndex,
            const Vector3 &translation, const Quaternion &rotation,
            const Vector3 &scaling, const Matrix4 &offsetMatrix);

    private:
        TPROPERTY(RTTRFuncName="ParentIndex", RTTRFuncType="setter")
        void setParent(uint16_t parentIndex)
        {
            mParentIndex = parentIndex;
        }

        TPROPERTY(RTTRFuncName="Translation", RTTRFuncType="setter")
        void setTranslation(const Vector3 &translation)
        {
            mTranslation = translation;
        }

        TPROPERTY(RTTRFuncName="Rotation", RTTRFuncType="setter")
        void setRotation(const Quaternion &rotation)
        {
            mRotation = rotation;
        }

        TPROPERTY(RTTRFuncName="Scaling", RTTRFuncType="setter")
        void setScaling(const Vector3 &scaling)
        {
            mScaling = scaling;
        }
        
        TPROPERTY(RTTRFuncName="OffsetMatrix", RTTRFuncType="setter")
        void setOffsetMatrix(const Matrix4 &offsetMatrix)
        {
            mOffsetMatrix = offsetMatrix;
        }
        
    protected:
        /// 骨骼名称
        String mName {"#Bone"};
        
        /// 父骨骼索引
        uint16_t mParentIndex {kInvalidIndex};

        /// 父骨骼空间下的位置
        Vector3 mTranslation {};
        /// 父骨骼空间下的旋转
        Quaternion mRotation {};
        /// 父骨骼空间下的缩放
        Vector3 mScaling {1.0f, 1.0f, 1.0f};

        /// 骨骼偏移矩阵
        Matrix4 mOffsetMatrix {false};
    };
}


#endif    /*__T3D_BONE_NODE_H__*/
