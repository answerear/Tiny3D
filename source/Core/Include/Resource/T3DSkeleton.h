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

#ifndef __T3D_SKELETON_H__
#define __T3D_SKELETON_H__


#include "Resource/T3DResource.h"
#include "Animation/T3DBoneNode.h"


namespace Tiny3D
{
    /**
     * \brief 骨架资源
     */
    TCLASS()
    class T3D_ENGINE_API Skeleton : public Resource
    {
        TRTTI_ENABLE(Resource)
        TRTTI_FRIEND
        
    public:
        static SkeletonPtr create(const String &name);

        static SkeletonPtr create(const String &name, Bones &&bones);

        /**
         * \brief 析构函数
         */
        ~Skeleton() override;

        Type getType() const override;

        TPROPERTY(RTTRFuncName="Bones", RTTRFuncType="getter")
        const Bones &getBones() const
        {
            return mBones;
        }

    protected:
        Skeleton() = default;
        
        Skeleton(const String &name);

        Skeleton(const String &name, Bones &&bones);
        
        ResourcePtr clone() const override;

        void cloneProperties(const Resource * const src) override;

        TResult onCreate() override;
        
        TResult onLoad(Archive *archive) override;

    private:
        TPROPERTY(RTTRFuncName="Bones", RTTRFuncType="setter")
        void setBones(const Bones &bones)
        {
            mBones = bones;
        }
        
    protected:
        /// 骨骼 hierarchy
        Bones mBones {};
    };
}


#endif    /*__T3D_SKELETON_H__*/
