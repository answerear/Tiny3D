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

        /**
         * 判断组件是否为场景树节点组件（Transform 系）
         */
        bool isTransformComponent(const Component *component)
        {
            const RTTRType type = RTTRType::get(*component);
            const RTTRType transformType = RTTRType::get<TransformNode>();

            return (type == transformType || type.is_derived_from(transformType));
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

            RTTRObject instance(*comp);
            const String title
                = instance.get_derived_type().get_name().to_string();

            // 不同组件可能存在同名属性，用组件地址隔离 ImGui 控件 ID
            ImGui::PushID(comp);

            if (ImGui::CollapsingHeader(title.c_str(),
                ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImPropertyDrawer::drawObject(instance))
                {
                    notifySceneModified();
                }
            }

            ImGui::PopID();
        }
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
