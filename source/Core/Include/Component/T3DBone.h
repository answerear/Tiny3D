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

#ifndef __T3D_BONE_H__
#define __T3D_BONE_H__


#include "Component/T3DComponent.h"


namespace Tiny3D
{
    /**
     * \brief 骨骼节点组件，挂载于骨骼 GameObject 上，存储蒙皮用的偏移矩阵
     */
    TCLASS()
    class T3D_ENGINE_API Bone : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND

    public:
        /// 无效骨骼索引常量（0xFFFF）
        static const uint16_t kInvalidIndex;
        
        /**
         * \brief 创建带新生成 UUID 的 Bone 实例
         * \return 新建的 Bone 智能指针
         */
        static BonePtr create();

        /// 析构
        ~Bone() override = default;
        
        /**
         * \brief 克隆当前 Bone
         * \return 克隆成功返回新组件；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;
        
        /// 返回骨骼偏移矩阵（bind pose 逆矩阵）
        TPROPERTY(RTTRFuncName="OffsetMatrix", RTTRFuncType="getter")
        const Matrix4 &getOffsetMatrix() const
        {
            return mOffsetMatrix;
        }
        
        /**
         * \brief 设置骨骼偏移矩阵
         * \param [in] offsetMatrix : 新的偏移矩阵
         */
        TPROPERTY(RTTRFuncName="OffsetMatrix", RTTRFuncType="setter")
        void setOffsetMatrix(const Matrix4 &offsetMatrix)
        {
            mOffsetMatrix = offsetMatrix;
        }
        
    protected:
        /// 默认构造
        Bone() = default;
        
        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件唯一标识
         */
        Bone(const UUID &uuid);

        /**
         * \brief 从源 Bone 拷贝 mOffsetMatrix
         * \param [in] src : 源组件
         * \return 成功返回 T3D_OK
         */
        TResult cloneProperties(const Component * const src) override;
        
    protected:
        /// 骨骼偏移矩阵
        Matrix4 mOffsetMatrix {false};
    };
}


#endif    /*__T3D_BONE_H__*/
