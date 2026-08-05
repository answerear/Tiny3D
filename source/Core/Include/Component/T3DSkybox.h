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
     * \brief 天空盒组件，提供背景渲染材质
     * \remarks 挂在相机上时仅影响该相机；挂在场景根节点上作为全局天空盒。
     */
    TCLASS()
    class T3D_ENGINE_API Skybox : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND

    public:
        /**
         * \brief 创建带新生成 UUID 的 Skybox 实例
         * \return 新建的 Skybox 智能指针
         */
        static SkyboxPtr create();

        /// 析构
        ~Skybox() override;

        /**
         * \brief enabled 开关对本组件有效
         * \return 恒返回 true；关闭后不参与天空盒解析
         */
        bool supportsEnabled() const override { return true; }

        /**
         * \brief 克隆当前 Skybox
         * \return 克隆成功返回新组件；cloneProperties 失败返回 nullptr
         */
        ComponentPtr clone() const override;

        /// 返回天空盒材质的 UUID
        TPROPERTY(RTTRFuncName="MaterialUUID", RTTRFuncType="getter")
        const UUID &getMaterialUUID() const { return mMaterialUUID; }

        /**
         * \brief 设置天空盒材质 UUID 并清空已缓存的材质对象
         * \param [in] uuid : 材质 UUID
         */
        TPROPERTY(RTTRFuncName="MaterialUUID", RTTRFuncType="setter")
        void setMaterialUUID(const UUID &uuid);

        /// 返回已加载的天空盒材质；未加载时为 nullptr
        Material *getMaterial() const { return mMaterial; }

        /**
         * \brief 直接设置天空盒材质并同步 UUID
         * \param [in] material : 材质对象，可为 nullptr（UUID 置为 INVALID）
         */
        void setMaterial(Material *material);

    protected:
        /// 默认构造
        Skybox() = default;

        /**
         * \brief 以指定 UUID 构造
         * \param [in] uuid : 组件唯一标识
         */
        Skybox(const UUID &uuid);

        /**
         * \brief 从源 Skybox 拷贝材质 UUID 与材质引用
         * \param [in] src : 源组件
         * \return 成功返回 T3D_OK
         */
        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 从 Archive 按 mMaterialUUID 加载天空盒材质
         * \param [in] archive : 资源包
         */
        void onLoadResource(Archive *archive) override;

        /**
         * \brief 销毁时清空材质引用
         */
        void onDestroy() override;

    protected:
        /// 天空盒材质的 UUID
        UUID        mMaterialUUID {UUID::INVALID};
        /// 天空盒材质对象
        MaterialPtr mMaterial {nullptr};
    };
}


#endif  /*__T3D_SKYBOX_H__*/
