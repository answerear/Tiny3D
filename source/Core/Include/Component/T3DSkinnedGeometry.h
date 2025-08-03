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

#ifndef __T3D_SKINNED_GEOMETRY_H__
#define __T3D_SKINNED_GEOMETRY_H__


#include "Component/T3DGeometry.h"


namespace Tiny3D
{
    TCLASS()
    class T3D_ENGINE_API SkinnedGeometry : public Geometry
    {
        TRTTI_ENABLE(Geometry)
        TRTTI_FRIEND
        
    public:
        ~SkinnedGeometry() override;

        ComponentPtr clone() const override;
    
        /**
         * @brief 生成所有子 game object
         * @return 
         */
        TResult populateAllChildren();

        const BoneGameObjects &getAllBones() const { return mAllBones; }

        TPROPERTY(RTTRFuncName="DefaultClip", RTTRFuncType="getter")
        const String &getDefaultClipName() const { return mDefaultClipName; }

        TPROPERTY(RTTRFuncName="DefaultClip", RTTRFuncType="setter")
        void setDefaultClipName(const String &name) { mDefaultClipName = name; }

        bool play(const String &clipName);
        
    protected:
        SkinnedGeometry() = default;

        SkinnedGeometry(const UUID &uuid);

        TResult cloneProperties(const Component * const src) override;

        void onPostLoad() override;

        void onLoadResource(Archive *archive) override;

        void onStart() override;

        void onUpdate() override;

        void onDestroy() override;
        
        /// 生成渲染用的材质
        void generateRenderMaterial() override;
        
    protected:
        AnimationPlayerPtr mAnimationPlayer {nullptr};

        BoneGameObjects mAllBones {};

        String mDefaultClipName {};
    };
}


#endif  /*__T3D_SKINNED_GEOMETRY_H__*/
