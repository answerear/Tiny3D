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

#ifndef __T3D_SKYBOX_H__
#define __T3D_SKYBOX_H__


#include "Component/T3DComponent.h"


namespace Tiny3D
{
    /**
     * \brief 天空盒组件
     *
     * 对标 Unity 的 Skybox 组件：挂在相机上时只影响该相机，挂在场景根节点上时
     * 作为全局天空盒（等价于 Unity 的 RenderSettings.skybox）。
     */
    TCLASS()
    class T3D_ENGINE_API Skybox : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND

    public:
        static SkyboxPtr create();

        ~Skybox() override;

        /// 关掉后本组件提供的天空盒不参与解析
        bool supportsEnabled() const override { return true; }

        ComponentPtr clone() const override;

        TPROPERTY(RTTRFuncName="MaterialUUID", RTTRFuncType="getter")
        const UUID &getMaterialUUID() const { return mMaterialUUID; }

        TPROPERTY(RTTRFuncName="MaterialUUID", RTTRFuncType="setter")
        void setMaterialUUID(const UUID &uuid);

        Material *getMaterial() const { return mMaterial; }

        void setMaterial(Material *material);

    protected:
        Skybox() = default;

        Skybox(const UUID &uuid);

        TResult cloneProperties(const Component * const src) override;

        void onLoadResource(Archive *archive) override;

        void onDestroy() override;

    protected:
        /// 天空盒材质的 UUID
        UUID        mMaterialUUID {UUID::INVALID};
        /// 天空盒材质对象
        MaterialPtr mMaterial {nullptr};
    };
}


#endif  /*__T3D_SKYBOX_H__*/
