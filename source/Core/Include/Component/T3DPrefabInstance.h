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

#ifndef __T3D_PREFAB_INSTANCE_H__
#define __T3D_PREFAB_INSTANCE_H__


#include "Component/T3DComponent.h"
#include "Component/T3DPropertyModification.h"


namespace Tiny3D
{
    /**
     * \brief 场景中链接到源 Prefab 资产的实例组件
     * \remarks 挂在实例根 GameObject 上；用模板 UUID 映射实例对象并保存属性覆盖
     */
    TCLASS()
    class T3D_ENGINE_API PrefabInstance : public Component
    {
        TRTTI_ENABLE(Component)
        TRTTI_FRIEND

    public:
        using TemplateToInstanceMap = TUnorderedMap<UUID, Object*, UUIDHash, UUIDEqual>;

        static PrefabInstancePtr create();

        ~PrefabInstance() override = default;

        ComponentPtr clone() const override;

        TPROPERTY(RTTRFuncName="SourcePrefabUUID", RTTRFuncType="getter")
        const UUID &getSourcePrefabUUID() const { return mSourcePrefabUUID; }

        TPROPERTY(RTTRFuncName="SourcePrefabUUID", RTTRFuncType="setter")
        void setSourcePrefabUUID(const UUID &uuid) { mSourcePrefabUUID = uuid; }

        TPROPERTY(RTTRFuncName="Modifications", RTTRFuncType="getter")
        const PropertyModifications &getModifications() const { return mModifications; }

        TPROPERTY(RTTRFuncName="Modifications", RTTRFuncType="setter")
        void setModifications(const PropertyModifications &mods) { mModifications = mods; }

        TPROPERTY(RTTRFuncName="MappedTemplateUUIDs", RTTRFuncType="getter")
        const TArray<UUID> &getMappedTemplateUUIDs() const { return mMappedTemplateUUIDs; }

        TPROPERTY(RTTRFuncName="MappedTemplateUUIDs", RTTRFuncType="setter")
        void setMappedTemplateUUIDs(const TArray<UUID> &uuids) { mMappedTemplateUUIDs = uuids; }

        TPROPERTY(RTTRFuncName="MappedInstanceUUIDs", RTTRFuncType="getter")
        const TArray<UUID> &getMappedInstanceUUIDs() const { return mMappedInstanceUUIDs; }

        TPROPERTY(RTTRFuncName="MappedInstanceUUIDs", RTTRFuncType="setter")
        void setMappedInstanceUUIDs(const TArray<UUID> &uuids) { mMappedInstanceUUIDs = uuids; }

        TemplateToInstanceMap &getTemplateToInstanceMap() { return mTemplateToInstance; }
        const TemplateToInstanceMap &getTemplateToInstanceMap() const { return mTemplateToInstance; }

        void setTemplateToInstanceMap(const TemplateToInstanceMap &map);

        void clearTemplateToInstanceMap();

        /**
         * \brief 根据序列化的 UUID 对重建运行时 Object* 映射
         */
        void rebuildRuntimeMapFromSerializedUUIDs();

        Object *resolveInstanceObject(const UUID &templateUUID) const;

        void recordOverride(const PropertyModification &mod);

        void clearOverride(const UUID &targetObjectUUID, const String &propertyPath);

        void clearAllOverrides() { mModifications.clear(); }

        /**
         * \brief 对比源 Prefab 与当前实例，刷新 mModifications
         */
        TResult recordOverridesFromSource();

        TResult applyOverridesToInstance();

#if defined(T3D_EDITOR)
        TResult applyToPrefabAsset();

        TResult revert(const UUID &targetObjectUUID, const String &propertyPath);

        TResult revertAll();

        enum class UnpackMode
        {
            kOutermostRootOnly,
            kCompletely
        };

        static TResult unpackPrefab(GameObject *instanceRoot, UnpackMode mode);
#endif

    protected:
        PrefabInstance() = default;

        PrefabInstance(const UUID &uuid);

        TResult cloneProperties(const Component * const src) override;

        /**
         * \brief 解析源 Prefab 资产
         * \param [out] holder : 持有按 UUID 加载出来的资源引用，防止调用方拿到悬垂指针
         * \return 命中返回 Prefab 裸指针，否则返回 nullptr
         * \remarks 先查资源管理器缓存，未命中才按 UUID 读档；FileSystem 这类不支持
         *          UUID 寻址的档案上，读档会失败并返回 nullptr
         */
        Prefab *resolveSourcePrefab(PrefabPtr &holder) const;

        void rebuildTemplateMapFromHierarchy();

    protected:
        UUID mSourcePrefabUUID {};
        PropertyModifications mModifications {};
        TArray<UUID> mMappedTemplateUUIDs {};
        TArray<UUID> mMappedInstanceUUIDs {};
        TemplateToInstanceMap mTemplateToInstance {};
    };
}


#endif  /*__T3D_PREFAB_INSTANCE_H__*/