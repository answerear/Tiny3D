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


#include "UIInspectorWindow.h"
#include "EditorEventDefine.h"
#include "GUIExtension/ImGuiExtension.h"
#include <algorithm>
#include <cstring>


namespace Tiny3D
{
    NS_BEGIN(Editor)

    namespace
    {
        /// game object 名称编辑缓冲区大小
        constexpr size_t kNameBufferSize = 256;

        /// Add Component 候选菜单的 ImGui 标识
        const char * const kAddComponentPopup = "AddComponentPopup";
        /// 组件标题右键菜单的 ImGui 标识
        const char * const kComponentContextMenu = "ComponentContextMenu";

        /// Add Component 候选列表可见的行数，超出后在弹窗内滚动
        constexpr int32_t kCandidateVisibleRows = 10;

        /**
         * 判断组件是否为场景树节点组件（Transform 系）
         */
        bool isTransformComponent(const Component *component)
        {
            const RTTRType type = RTTRType::get(*component);
            const RTTRType transformType = RTTRType::get<TransformNode>();

            return (type == transformType || type.is_derived_from(transformType));
        }

        /**
         * 取类名的最后一段作为界面显示名。
         * RTTR 注册的是带命名空间的全名（如 "Tiny3D::Camera"），直接显示过于冗长
         */
        String toDisplayName(const String &fullName)
        {
            const size_t pos = fullName.rfind("::");

            return (pos == String::npos) ? fullName : fullName.substr(pos + 2);
        }
    }

    //--------------------------------------------------------------------------

    TResult UIInspectorWindow::onCreate()
    {
        ON_MEMBER(kEvtGameObjectSelected, UIInspectorWindow::onGameObjectSelected);

        return T3D_OK;
    }

    //--------------------------------------------------------------------------

    void UIInspectorWindow::onDestroy()
    {
        mSelectedGameObject = nullptr;

        unregisterAllEvent();

        UIDockingWindow::onDestroy();
    }

    //--------------------------------------------------------------------------

    bool UIInspectorWindow::onGameObjectSelected(EventParam *param, TINSTANCE sender)
    {
        if (param == nullptr)
        {
            mSelectedGameObject = nullptr;
            return true;
        }

        EventParamGameObjectSelected *para
            = static_cast<EventParamGameObjectSelected *>(param);
        mSelectedGameObject = para->arg1;

        return true;
    }

    //--------------------------------------------------------------------------

    void UIInspectorWindow::onGUI()
    {
        if (mSelectedGameObject == nullptr)
        {
            ImGui::TextDisabled("No game object selected.");
            return;
        }

        drawHeader();

        ImGui::Separator();

        drawComponents();

        ImGui::Separator();

        drawAddComponentButton();

        applyPendingActions();
    }

    //--------------------------------------------------------------------------

    void UIInspectorWindow::drawHeader()
    {
        GameObject *gameObject = mSelectedGameObject;

        bool active = gameObject->isActive();
        if (ImGui::Checkbox("##Active", &active))
        {
            gameObject->setActive(active);
            notifySceneModified();
        }

        ImGui::SameLine();

        char name[kNameBufferSize] = { 0 };
        const String &current = gameObject->getName();
        const size_t length = std::min(current.length(), kNameBufferSize - 1);
        ::memcpy(name, current.c_str(), length);

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::InputText("##Name", name, kNameBufferSize))
        {
            // setName() 是 protected，只能借助反射写回
            RTTRObject instance(*gameObject);
            rttr::property prop
                = instance.get_derived_type().get_property("Name");

            if (prop.is_valid() && prop.set_value(instance, String(name)))
            {
                notifySceneModified();
            }
        }

        bool visible = gameObject->isVisible();
        if (ImGui::Checkbox("Visible", &visible))
        {
            gameObject->setVisible(visible);
            notifySceneModified();
        }

        ImGui::TextDisabled("UUID : %s", gameObject->getUUID().toString().c_str());
    }

    //--------------------------------------------------------------------------

    void UIInspectorWindow::drawComponents()
    {
        GameObject *gameObject = mSelectedGameObject;

        TArray<ComponentPtr> components = gameObject->getComponents<Component>();

        // 组件存放在无序容器里，这里固定 Transform 在最前、其余按类名排序，
        // 避免面板上的组件次序在不同场景加载之间发生跳变
        std::sort(components.begin(), components.end(),
            [](const ComponentPtr &lhs, const ComponentPtr &rhs)
            {
                const bool lhsIsTransform = isTransformComponent(lhs);
                const bool rhsIsTransform = isTransformComponent(rhs);

                if (lhsIsTransform != rhsIsTransform)
                {
                    return lhsIsTransform;
                }

                return RTTRType::get(*lhs).get_name().to_string()
                    < RTTRType::get(*rhs).get_name().to_string();
            });

        for (auto &component : components)
        {
            Component *comp = component;
            if (comp == nullptr)
            {
                continue;
            }

            drawComponent(comp);
        }
    }

    //--------------------------------------------------------------------------

    void UIInspectorWindow::drawComponent(Component *component)
    {
        RTTRObject instance(*component);
        const String title
            = toDisplayName(instance.get_derived_type().get_name().to_string());

        // 不同组件可能存在同名属性，用组件地址隔离 ImGui 控件 ID
        ImGui::PushID(component);

        const bool expanded = ImGui::CollapsingHeader(title.c_str(),
            ImGuiTreeNodeFlags_DefaultOpen);

        // 右键菜单必须紧跟在标题控件之后，才会挂到该标题上
        if (ImGui::BeginPopupContextItem(kComponentContextMenu))
        {
            if (ImGui::MenuItem("Reset"))
            {
                mPendingResetComponent = component;
            }

            // transform 承载了场景树的父子关系，移除会让子节点失去挂点，
            // 与 Unity 一致禁止移除
            const bool removable = !isTransformComponent(component);

            if (ImGui::MenuItem("Remove Component", nullptr, false, removable))
            {
                mPendingRemoveComponent = component;
            }

            ImGui::EndPopup();
        }

        if (expanded)
        {
            if (ImPropertyDrawer::drawObject(instance))
            {
                notifySceneModified();

                // 资源引用字段存的是资产 UUID，组件只在 onLoadResource 里按 UUID 取
                // 资产，光改 UUID 画面不会变，需要让组件重新加载一次资源
                if (ImPropertyDrawer::wasAssetReferenceChanged())
                {
                    mPendingReloadComponent = component;
                }
            }
        }

        ImGui::PopID();
    }

    //--------------------------------------------------------------------------

    void UIInspectorWindow::drawAddComponentButton()
    {
        if (ImGui::Button("Add Component", ImVec2(-FLT_MIN, 0.0f)))
        {
            collectComponentCandidates();
            mComponentFilter.Clear();

            ImGui::OpenPopup(kAddComponentPopup);
        }

        if (!ImGui::BeginPopup(kAddComponentPopup))
        {
            return;
        }

        // 菜单弹出的首帧把键盘焦点放到搜索框上，省去一次点击
        if (ImGui::IsWindowAppearing())
        {
            ImGui::SetKeyboardFocusHere();
        }

        mComponentFilter.Draw("##ComponentFilter");

        const GameObject *gameObject = mSelectedGameObject;
        const float listHeight
            = ImGui::GetTextLineHeightWithSpacing() * kCandidateVisibleRows;

        // BeginChild 与 EndChild 必须无条件配对，不能按返回值决定
        ImGui::BeginChild("##CandidateList", ImVec2(0.0f, listHeight));

        for (const auto &candidate : mComponentCandidates)
        {
            if (!mComponentFilter.PassFilter(candidate.displayName.c_str()))
            {
                continue;
            }

            // 一个 game object 上只能挂一个 transform 系组件，已有则置灰，
            // 避免点了之后 addComponent 静默失败
            const bool enabled = !candidate.isTransform
                || gameObject->getTransformNode() == nullptr;

            ImGui::BeginDisabled(!enabled);

            if (ImGui::Selectable(candidate.displayName.c_str()))
            {
                mPendingAddComponent = candidate.fullName;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndDisabled();
        }

        ImGui::EndChild();
        ImGui::EndPopup();
    }

    //--------------------------------------------------------------------------

    void UIInspectorWindow::collectComponentCandidates()
    {
        mComponentCandidates.clear();

        const RTTRType transformType = RTTRType::get<TransformNode>();
        const RTTRType uuidType = RTTRType::get<UUID>();

        for (const auto &type : RTTRType::get<Component>().get_derived_classes())
        {
            // GameObject::addComponent 是用带 UUID 的构造函数创建组件的，
            // 抽象基类（Component / Light / Bound 等）不会注册该构造函数，
            // 因此这一条判断同时也把不可实例化的类型排除掉了
            if (!type.get_constructor({ uuidType }).is_valid())
            {
                continue;
            }

            ComponentCandidate candidate;
            candidate.fullName = type.get_name().to_string();
            candidate.displayName = toDisplayName(candidate.fullName);
            candidate.isTransform = type.is_derived_from(transformType);

            mComponentCandidates.emplace_back(std::move(candidate));
        }

        std::sort(mComponentCandidates.begin(), mComponentCandidates.end(),
            [](const ComponentCandidate &lhs, const ComponentCandidate &rhs)
            {
                return lhs.displayName < rhs.displayName;
            });
    }

    //--------------------------------------------------------------------------

    void UIInspectorWindow::applyPendingActions()
    {
        GameObject *gameObject = mSelectedGameObject;

        if (!mPendingAddComponent.empty())
        {
            const String name = mPendingAddComponent;
            mPendingAddComponent.clear();

            if (gameObject->addComponent(name) != nullptr)
            {
                notifySceneModified();
            }
        }

        if (mPendingRemoveComponent != nullptr)
        {
            const ComponentPtr component = mPendingRemoveComponent;
            mPendingRemoveComponent = nullptr;

            const String name = RTTRType::get(*component).get_name().to_string();

            if (gameObject->removeComponent(name) == T3D_OK)
            {
                notifySceneModified();
            }
        }

        if (mPendingResetComponent != nullptr)
        {
            const ComponentPtr component = mPendingResetComponent;
            mPendingResetComponent = nullptr;

            if (resetComponent(component))
            {
                notifySceneModified();
            }
        }

        if (mPendingReloadComponent != nullptr)
        {
            const ComponentPtr component = mPendingReloadComponent;
            mPendingReloadComponent = nullptr;

            // 走资源门面挂着的工程搜索链，与编辑器其它加载资源的地方保持一致。
            // 没有档案（工程尚未打开）时不能重载：组件会因为取不到资产而清空自己
            // 已有的资源引用
            Archive * const archive = T3D_ASSET_MGR.getArchive();

            if (archive != nullptr)
            {
                component->onLoadResource(archive);
            }
            else
            {
                EDITOR_LOG_WARNING("No archive mounted, skip reloading resources of"
                    " the component !");
            }
        }
    }

    //--------------------------------------------------------------------------

    bool UIInspectorWindow::resetComponent(Component *component)
    {
        const RTTRType type = RTTRType::get(*component);

        // 造一个临时实例来读取各属性的默认值。这里刻意用默认构造函数：只有带 UUID
        // 的构造函数才会把组件登记进 Component 的静态表，默认构造函数不会，所以
        // 临时对象随智能指针析构即可，不会在引擎里留下残留
        RTTRVariant var = type.create();

        bool ok = false;
        Component *raw = var.convert<Component *>(&ok);

        if (!ok || raw == nullptr)
        {
            return false;
        }

        const ComponentPtr defaults = raw;

        RTTRObject dst(*component);
        RTTRObject src(*raw);
        bool changed = false;

        for (auto prop : type.get_properties())
        {
            // UUID 是组件身份，重置掉会切断其它数据对它的引用
            if (prop.get_name() == "UUID")
            {
                continue;
            }

            if (prop.is_readonly())
            {
                continue;
            }

            // 容器属性里放的是结构性数据（如 TransformNode 的 Children 保存了子节点
            // UUID），恢复成默认的空容器会切断场景树。这些属性 inspector 目前也不支持
            // 编辑，等 Phase 4 支持容器编辑后再重新评估要不要放开
            const RTTRType propType = prop.get_type();

            if (propType.is_sequential_container()
                || propType.is_associative_container())
            {
                continue;
            }

            RTTRVariant value = prop.get_value(src);

            if (value.is_valid() && prop.set_value(dst, value))
            {
                changed = true;
            }
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    void UIInspectorWindow::notifySceneModified()
    {
        EventParamModifyScene param(true);
        postEvent(kEvtModifyScene, &param);
    }

    //--------------------------------------------------------------------------
    NS_END
}
