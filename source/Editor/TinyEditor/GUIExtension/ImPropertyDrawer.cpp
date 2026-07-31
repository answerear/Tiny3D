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


#include "GUIExtension/ImPropertyDrawer.h"
#include "ProjectManager.h"


namespace Tiny3D
{
    NS_BEGIN(Editor)

    namespace
    {
        /// 数值拖动控件的默认步长
        constexpr float kDragSpeed = 0.01f;
        /// 角度拖动控件的步长，单位为度
        constexpr float kAngleDragSpeed = 0.5f;
        /// 字符串编辑缓冲区大小
        constexpr size_t kTextBufferSize = 512;
        /// 复合值类型递归展开的层数上限，防止异常的反射注册造成过深递归
        constexpr int32_t kMaxCompoundDepth = 4;
        /// 容器元素个数上限，避免在 Size 框里误输入一个巨大的值直接吃掉内存
        constexpr size_t kMaxContainerSize = 4096;
        /// 资产选择器候选列表的可见行数
        constexpr float kAssetVisibleRows = 12.0f;

        /// 角度类控件的显示格式，标注单位避免与弧度混淆
        const char * const kAngleFormat = "%.2f deg";

        /**
         * 元数据 key。
         *
         * 类型必须是 String，不能是 const char *：生成代码里的 key 是字符串字面量，
         * RTTR 把一维 char 数组存成 std::string，而 const char * 会原样存成指针。
         * 查表是拿 variant 逐个比 key，这两种类型既不相等又互相转不过去，用
         * const char * 去查一律取不到值，且编译与运行期都不会报错。
         *
         * 标注写在属性 getter 的 TPROPERTY 标签里，例如：
         *   TPROPERTY(RTTRFuncName="FovY", RTTRFuncType="getter", "RANGE_MIN"=1, "RANGE_MAX"=179)
         *
         * 三条来自预处理器的约束：
         * - key 要带引号。标签里的 key 会被原样搬进生成代码，不带引号会变成未定义的标识符
         * - 标注要写在 getter 上。属性由 getter 与 setter 合并注册，只有 getter 的标注
         *   会被写进生成代码，标在 setter 上不生效
         * - 整个标签必须写成一行。跨行的标签会被漏掉，连带整个属性都不会被注册
         *
         * Description 是引擎里既有的属性文档，直接当提示用，无需另加标注。
         */
        /// 与序列化层共用，标记不参与序列化的属性
        const String kMetaNoSerialize = "NO_SERIALIZE";

        const String kMetaDescription = "Description";
        const String kMetaTooltip = "TOOLTIP";
        const String kMetaRangeMin = "RANGE_MIN";
        const String kMetaRangeMax = "RANGE_MAX";
        const String kMetaHeader = "HEADER";
        const String kMetaDisplayName = "DISPLAY_NAME";
        const String kMetaHideInInspector = "HIDE_IN_INSPECTOR";

        /// 资产选择器弹窗的 ID，外层已按属性名 PushID，各属性之间不会串
        const char * const kAssetPickerPopup = "##AssetPicker";

        /**
         * 资产拖拽的 payload 类型标识，负载内容为资产 UUID。
         *
         * 引用字段都会注册成该类型的拖拽接收方，但 project 面板目前还没有实现对应的
         * 拖拽源（编辑器里尚无任何 BeginDragDropSource），所以接收方现在不会被触发；
         * 等资产列表补上拖拽源后即可生效，无需再改这里。
         */
        const char * const kAssetPayload = "T3D_ASSET_UUID";

        /**
         * 拖动过程中缓存的欧拉角。
         *
         * 四元数反解欧拉角的结果并不唯一（万向锁与角度环绕处会取到等价的另一组值），
         * 若每帧都从四元数重新反解作为控件的显示值，拖动到 pitch 超过 90 度一类的
         * 边界时角度会突跳并与用户的拖动方向相抗。因此控件处于激活状态期间沿用缓存
         * 的角度，松手后再回到从四元数反解。
         *
         * ImGui 同一时刻只有一个控件处于激活状态，所以只需缓存一份。
         */
        struct EulerCache
        {
            ImGuiID id {0};
            float   angles[3] {0.0f, 0.0f, 0.0f};
        };

        EulerCache sEulerCache;

        /**
         * 把字符串安全地拷贝进定长缓冲区，并保证以 '\0' 结尾
         */
        void copyToBuffer(char *buffer, size_t bufferSize, const String &text)
        {
            const size_t length = std::min(text.length(), bufferSize - 1);
            ::memcpy(buffer, text.c_str(), length);
            buffer[length] = '\0';
        }

        /**
         * 取属性上标注的元数据值
         * @return 未标注时返回无效变体。标签只写 key 不写值时预处理器生成的值是
         *         true，它只代表「标注过」而不携带内容，这里同样按无效处理，
         *         免得把它当文本显示成 "true" 或当数值用成 1
         */
        RTTRVariant metaValue(const rttr::property &prop, const String &key)
        {
            RTTRVariant md = prop.get_metadata(key);

            return (md.get_type() == RTTRType::get<bool>()) ? RTTRVariant() : md;
        }

        /// 读取字符串型元数据，未标注或标注的值不是文本时返回空串
        String metaString(const rttr::property &prop, const String &key)
        {
            const RTTRVariant md = metaValue(prop, key);

            if (!md.is_valid())
            {
                return String();
            }

            bool ok = false;
            const String text = md.to_string(&ok);

            return ok ? text : String();
        }

        /**
         * 读取数值型元数据
         * @param [out] number : 读取成功时写入标注的数值
         * @return 未标注或标注的值取不到数值时返回 false
         */
        bool metaNumber(const rttr::property &prop, const String &key, double &number)
        {
            const RTTRVariant md = metaValue(prop, key);

            if (!md.is_valid())
            {
                return false;
            }

            bool ok = false;
            const double value = md.to_double(&ok);

            if (ok)
            {
                number = value;
            }

            return ok;
        }

        /**
         * 取标签里用于显示的部分。
         * 标签可能带 "##" 后缀来固定控件 ID（见 drawProperty 对显示名的处理）。
         * 交给 ImGui 控件的标签由控件自己截断，只有自行输出标签文字的地方要先过一遍
         */
        String displayText(const String &label)
        {
            const size_t pos = label.find("##");

            return (pos == String::npos) ? label : label.substr(0, pos);
        }

        /**
         * 本次 drawObject 期间是否改写过资源引用属性。
         * 在 depth 为 0 的 drawObject 入口清零，由 wasAssetReferenceChanged 读取。
         */
        bool sAssetReferenceChanged = false;

        /// 去掉反射类型名里的命名空间前缀，只留下类名用于显示
        String stripNamespace(const String &name)
        {
            const size_t pos = name.rfind("::");

            return (pos == String::npos) ? name : name.substr(pos + 2);
        }

        /// 资产选择器里的一个候选资产
        struct AssetCandidate
        {
            UUID    uuid;
            String  name;
        };

        /**
         * 资产选择器的候选列表与搜索词。
         *
         * 候选项在弹窗打开的那一帧快照，之后不再回头访问资产树：project 面板在窗口
         * 重新获得焦点时会整棵重建资产树（见 ProjectManager::applicationFocusGained），
         * 缓存 AssetNode * 会变成野指针，因此这里只留下 UUID 与名字的副本。
         * ImGui 同一时刻只会打开一个弹窗，所以只需一份。
         */
        TArray<AssetCandidate> sAssetCandidates;
        ImGuiTextFilter sAssetFilter;

        /**
         * 判断资产节点的种类能否作为所需种类的引用目标
         */
        bool matchesAssetType(Meta::Type required, Meta::Type actual)
        {
            if (required == actual)
            {
                return true;
            }

            // 着色器在工程里既可能是编译产物，也可能是 shaderlab 源码，两者都能被引用
            return required == Meta::kShader && actual == Meta::kShaderLab;
        }

        /// 取资产节点用于显示的名字，标题为空时退回文件名
        String assetDisplayName(AssetNode *node)
        {
            return node->getTitle().empty() ? node->getFilename() : node->getTitle();
        }

        /// 递归收集资产树上指定种类的资产
        void collectAssets(AssetNode *node, Meta::Type assetType,
            TArray<AssetCandidate> &candidates)
        {
            if (node == nullptr)
            {
                return;
            }

            Meta * const meta = node->getMeta();

            if (meta != nullptr && matchesAssetType(assetType, meta->getType()))
            {
                AssetCandidate candidate;
                candidate.uuid = meta->getUUID();
                candidate.name = assetDisplayName(node);

                candidates.emplace_back(std::move(candidate));
            }

            for (auto child : node->getChildren())
            {
                collectAssets(child, assetType, candidates);
            }
        }

        /**
         * 需要检索的资产树根节点集合
         * @remarks 工程资产与内置资源挂在不同档案上，资产树也分成两棵（见
         *          ProjectManager::mountAssetArchives）。按 UUID 反查与列举候选都必须
         *          覆盖两棵树：层级面板创建的 Cube / Sphere 一类对象引用的是内置网格，
         *          只查工程资产树会把它们误判成引用丢失。
         *          顺序与搜索链的优先级一致，工程资产在前
         */
        TArray<AssetNode *> assetRoots()
        {
            TArray<AssetNode *> roots;

            AssetNode * const assets = PROJECT_MGR.getAssetRoot();

            if (assets != nullptr)
            {
                roots.emplace_back(assets);
            }

            AssetNode * const builtin = PROJECT_MGR.getBuiltinAssetRoot();

            if (builtin != nullptr)
            {
                roots.emplace_back(builtin);
            }

            return roots;
        }

        /// 按 UUID 反查资产名，找不到返回空串
        String findAssetName(AssetNode *node, const UUID &uuid)
        {
            if (node == nullptr)
            {
                return String();
            }

            Meta * const meta = node->getMeta();

            if (meta != nullptr && meta->getUUID() == uuid)
            {
                return assetDisplayName(node);
            }

            for (auto child : node->getChildren())
            {
                const String name = findAssetName(child, uuid);

                if (!name.empty())
                {
                    return name;
                }
            }

            return String();
        }

        /**
         * 按属性名推断 UUID 属性引用的资产种类
         * @return 推断不出时返回 Meta::kUnknown
         * @remarks 引擎里的 UUID 属性并非都指向资产：组件自身的身份 UUID（属性名就叫
         *          UUID）、指向场景内对象的 UUID（RootGameObjectUUID / RootBoneUUID）
         *          都不该弹出资产选择器。因此这里采取白名单策略，只有名字命中下表的
         *          属性才认定为资源引用，其余退回只读展示。
         */
        Meta::Type inferAssetType(const String &propName)
        {
            struct Entry
            {
                const char *suffix;
                Meta::Type  type;
            };

            static const Entry kTable[] =
            {
                { "MeshUUID",       Meta::kMesh },
                { "TextureUUID",    Meta::kTexture },
                { "MaterialUUID",   Meta::kMaterial },
                { "ShaderUUID",     Meta::kShader },
                { "SkeletonUUID",   Meta::kSkeleton },
                { "AnimationUUID",  Meta::kAnimation },
                { "PrefabUUID",     Meta::kPrefab },
                { "SceneUUID",      Meta::kScene },
            };

            for (const auto &entry : kTable)
            {
                const size_t length = ::strlen(entry.suffix);

                // 按后缀匹配，这样 SkeletalAnimationUUID 一类的限定名也能命中
                if (propName.length() >= length
                    && propName.compare(propName.length() - length, length,
                        entry.suffix) == 0)
                {
                    return entry.type;
                }
            }

            return Meta::kUnknown;
        }

        /**
         * 把当前控件登记为资产拖拽的接收方
         * @param [out] uuid : 成功接收时填入拖入资产的 UUID
         * @return 本帧接收到拖放时返回 true
         */
        bool acceptAssetDrop(UUID &uuid)
        {
            bool dropped = false;

            if (ImGui::BeginDragDropTarget())
            {
                const ImGuiPayload * const payload
                    = ImGui::AcceptDragDropPayload(kAssetPayload);

                // 负载用 UUID 的字符串形式而非二进制结构：ImGui 的负载缓冲区是字节
                // 数组，按 UUID 结构直接取值存在对齐隐患，字符串形式也便于拖拽源构造
                if (payload != nullptr && payload->Data != nullptr
                    && payload->DataSize > 0)
                {
                    const String text(static_cast<const char *>(payload->Data),
                        static_cast<size_t>(payload->DataSize));

                    uuid.fromString(text);
                    dropped = true;
                }

                ImGui::EndDragDropTarget();
            }

            return dropped;
        }
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::wasAssetReferenceChanged()
    {
        return sAssetReferenceChanged;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawObject(const RTTRObject &obj, int32_t depth)
    {
        if (depth == 0)
        {
            sAssetReferenceChanged = false;
        }

        // 智能指针包装的对象需要先解包，否则拿不到真正的属性表
        const RTTRObject instance = obj.get_type().get_raw_type().is_wrapper()
            ? obj.get_wrapped_instance() : obj;

        if (!instance.is_valid())
        {
            return false;
        }

        bool changed = false;

        for (auto prop : instance.get_derived_type().get_properties())
        {
            changed = drawProperty(instance, prop, depth) || changed;
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    ImPropertyDrawer::UIHints ImPropertyDrawer::readUIHints(const rttr::property &prop)
    {
        UIHints hints;

        // TOOLTIP 是专为 inspector 写的说明，Description 是属性自身的文档，
        // 前者优先，没写时退回后者，这样引擎里已有的属性文档能直接变成提示
        hints.tooltip = metaString(prop, kMetaTooltip);

        if (hints.tooltip.empty())
        {
            hints.tooltip = metaString(prop, kMetaDescription);
        }

        hints.header = metaString(prop, kMetaHeader);
        hints.displayName = metaString(prop, kMetaDisplayName);

        double low = 0.0;
        double high = 0.0;

        // 只标了一个边界或区间是反的都定不出滑条量程，按没标注处理，仍用拖动控件
        if (metaNumber(prop, kMetaRangeMin, low)
            && metaNumber(prop, kMetaRangeMax, high)
            && low < high)
        {
            hints.hasRange = true;
            hints.rangeMin = low;
            hints.rangeMax = high;
        }

        return hints;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawProperty(const RTTRObject &obj, const rttr::property &prop,
        int32_t depth)
    {
        // 不参与序列化的属性改了也存不下来，直接不展示
        if (prop.get_metadata(kMetaNoSerialize))
        {
            return false;
        }

        // 属性显式标注了不在 inspector 呈现
        if (prop.get_metadata(kMetaHideInInspector))
        {
            return false;
        }

        const UIHints hints = readUIHints(prop);
        const String name = prop.get_name().to_string();

        // 显示名只换标签文字：ImGui 拿标签当控件 ID，直接整体替换会让同一对象里显示名
        // 相同的两个属性撞 ID，而 "##" 之后的部分只参与 ID 不参与显示
        const String label = hints.displayName.empty()
            ? name : hints.displayName + "##" + name;

        // 分组标题自成一行，放在属性之前
        if (!hints.header.empty())
        {
            ImGui::SeparatorText(hints.header.c_str());
        }

        // 提示要覆盖属性画出的全部控件，因此先把它们并成一个 ImGui 分组再判断悬停。
        // 只在确实有提示时分组，不给其余属性引入多余的布局层级
        const bool grouped = !hints.tooltip.empty();

        if (grouped)
        {
            ImGui::BeginGroup();
        }

        const bool changed = drawPropertyValue(obj, prop, label, hints, depth);

        if (grouped)
        {
            ImGui::EndGroup();

            // 提示用的悬停标志默认已含 AllowWhenDisabled，只读属性一样能弹出提示
            ImGui::SetItemTooltip("%s", hints.tooltip.c_str());
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawPropertyValue(const RTTRObject &obj,
        const rttr::property &prop, const String &label, const UIHints &hints,
        int32_t depth)
    {
        RTTRVariant value = prop.get_value(obj);

        if (!value.is_valid())
        {
            drawReadOnlyText(label, "<null>");
            return false;
        }

        const RTTRType type = prop.get_type();

        // 结构性属性只展示规模。这里不走 BeginDisabled 包住容器控件的做法：被禁用的
        // 折叠节点连展开都点不动，反而比摘要更少信息
        if (isStructuralProperty(prop))
        {
            drawContainerSummary(label, value);
            return false;
        }

        if (prop.is_readonly())
        {
            ImGui::BeginDisabled();
            drawValue(label, type, value, depth, hints);
            ImGui::EndDisabled();
            return false;
        }

        // UUID 属性分两类：对象自身的身份 UUID 不能改，指向资产的 UUID 走资产选择器。
        // 这里按属性名判断，不能用 label：显示名会把名字换成与资产种类无关的文案
        const Meta::Type assetType = (type == RTTRType::get<UUID>())
            ? inferAssetType(prop.get_name().to_string()) : Meta::kUnknown;
        const bool isAssetReference = (assetType != Meta::kUnknown);

        const bool changed = isAssetReference
            ? drawAssetReference(label, assetType, value)
            : drawValue(label, type, value, depth, hints);

        if (!changed || !prop.set_value(obj, value))
        {
            return false;
        }

        // 写回成功才登记，避免属性写入失败时还让调用方白跑一次资源重载
        if (isAssetReference)
        {
            sAssetReferenceChanged = true;
        }

        return true;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawValue(const String &label, const RTTRType &type,
        RTTRVariant &value, int32_t depth, const UIHints &hints)
    {
        if (type == RTTRType::get<bool>())
        {
            bool flag = value.to_bool();

            if (ImGui::Checkbox(label.c_str(), &flag))
            {
                value = flag;
                return true;
            }

            return false;
        }

        if (type.is_arithmetic())
        {
            return drawArithmetic(label, type, value, hints);
        }

        if (type.is_enumeration())
        {
            return drawEnumeration(label, type, value);
        }

        if (type == RTTRType::get<String>())
        {
            return drawString(label, value);
        }

        if (type == RTTRType::get<Vector2>())
        {
            return drawVector2(label, value);
        }

        if (type == RTTRType::get<Vector3>())
        {
            return drawVector3(label, value);
        }

        if (type == RTTRType::get<Vector4>())
        {
            return drawVector4(label, value);
        }

        if (type == RTTRType::get<Quaternion>())
        {
            return drawQuaternion(label, value);
        }

        if (type == RTTRType::get<Radian>())
        {
            return drawRadian(label, value, hints);
        }

        if (type == RTTRType::get<Degree>())
        {
            return drawDegree(label, value, hints);
        }

        if (type == RTTRType::get<ColorRGB>())
        {
            return drawColorRGB(label, value);
        }

        if (type == RTTRType::get<ColorRGBA>())
        {
            return drawColorRGBA(label, value);
        }

        // UUID 由引擎维护，不允许手工编辑，只作展示
        if (type == RTTRType::get<UUID>())
        {
            drawReadOnlyText(label, value.get_value<UUID>().toString());
            return false;
        }

        if (value.is_sequential_container())
        {
            return drawSequentialContainer(label, value, depth, hints);
        }

        if (value.is_associative_container())
        {
            // 关联容器增删要同时构造 key 与 value，键还需保证唯一，交互远比顺序容器
            // 复杂；引擎里也没有需要在 inspector 里编辑的关联容器属性，只展示条目数
            drawContainerSummary(label, value);
            return false;
        }

        // 裸指针与智能指针承载的是对象引用，而非可展开的结构体
        if (type.is_pointer() || type.is_wrapper())
        {
            return drawObjectReference(label, type, value);
        }

        // Viewport / Aabb / Obb 这类已注册的复合值类型，展开逐个编辑子属性
        if (isCompoundValue(type, depth))
        {
            return drawCompound(label, value, depth);
        }

        drawReadOnlyText(label, type.get_name().to_string());

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::isCompoundValue(const RTTRType &type, int32_t depth)
    {
        if (depth >= kMaxCompoundDepth)
        {
            return false;
        }

        // 指针与智能指针表示对象引用，已由 drawObjectReference 处理；派生自 Object 的
        // 类型是有独立身份的对象（资源等），不该被当成宿主的内联结构体展开编辑。
        // 这里重复挡一道，避免该函数被别处直接调用时漏判
        if (type.is_pointer() || type.is_wrapper()
            || type.is_derived_from(RTTRType::get<Object>()))
        {
            return false;
        }

        return type.is_class() && !type.get_properties().empty();
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawCompound(const String &label, RTTRVariant &value, int32_t depth)
    {
        bool changed = false;

        if (ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            // instance 直接指向 value 内部保存的对象，因此子属性的写入会落在
            // value 上，最终由调用方把整个 value 写回宿主对象
            const RTTRObject sub(value);

            for (auto prop : sub.get_derived_type().get_properties())
            {
                changed = drawProperty(sub, prop, depth + 1) || changed;
            }

            ImGui::TreePop();
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::isStructuralProperty(const rttr::property &prop)
    {
        // TransformNode 的 Children 存的是子节点 UUID，父子关系由层级面板与
        // TransformNode 自己维护（增删还要同步父子双方的挂接），从 inspector 改写
        // 会直接切断场景树
        const RTTRType declaring = prop.get_declaring_type();
        const RTTRType transform = RTTRType::get<TransformNode>();

        return prop.get_name() == "Children"
            && (declaring == transform || declaring.is_derived_from(transform));
    }

    //--------------------------------------------------------------------------

    void ImPropertyDrawer::drawContainerSummary(const String &label, const RTTRVariant &value)
    {
        if (value.is_sequential_container())
        {
            const auto view = value.create_sequential_view();
            drawReadOnlyText(label, std::to_string(view.get_size()) + " items");
            return;
        }

        if (value.is_associative_container())
        {
            const auto view = value.create_associative_view();
            drawReadOnlyText(label, std::to_string(view.get_size()) + " entries");
            return;
        }

        drawReadOnlyText(label, value.get_type().get_name().to_string());
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawSequentialContainer(const String &label,
        RTTRVariant &value, int32_t depth, const UIHints &hints)
    {
        auto view = value.create_sequential_view();

        if (!view.is_valid())
        {
            drawReadOnlyText(label, value.get_type().get_name().to_string());
            return false;
        }

        // 多维数组的元素本身还是数组，逐层展开只会得到难以理解的嵌套控件；
        // 递归过深同样只做摘要，避免异常的反射注册把界面拖垮
        if (view.get_rank() > 1 || depth >= kMaxCompoundDepth)
        {
            drawContainerSummary(label, value);
            return false;
        }

        bool changed = false;

        // 折叠节点的 ID 用属性名，元素个数只作为显示内容。若把个数拼进 ID，
        // 增删元素时 ID 会变，节点会被 ImGui 当成另一个节点而自动收起
        const bool expanded = ImGui::TreeNodeEx(label.c_str(),
            ImGuiTreeNodeFlags_SpanAvailWidth, "%s (%d)",
            displayText(label).c_str(), static_cast<int32_t>(view.get_size()));

        if (!expanded)
        {
            return false;
        }

        // 定长数组（如 Vector3[3]）改不了元素个数，只能编辑既有元素
        const bool resizable = view.is_dynamic();

        if (resizable)
        {
            int32_t size = static_cast<int32_t>(view.get_size());

            // 用回车提交而非逐次按键生效，避免输入中间态（如把 12 改成 2 的瞬间）
            // 就把元素截掉
            if (ImGui::InputInt("Size", &size, 1, 10,
                ImGuiInputTextFlags_EnterReturnsTrue))
            {
                const size_t clamped = std::min(
                    static_cast<size_t>(std::max(size, 0)), kMaxContainerSize);

                if (clamped != view.get_size() && view.set_size(clamped))
                {
                    changed = true;
                }
            }
        }

        const RTTRType elementType = view.get_value_type();

        // 遍历期间增删会让容器的迭代失效，先记下要删的下标，等元素画完再执行
        constexpr size_t kNoIndex = static_cast<size_t>(-1);
        size_t pendingErase = kNoIndex;

        for (size_t i = 0; i < view.get_size(); ++i)
        {
            ImGui::PushID(static_cast<int32_t>(i));

            // get_value 返回的是包了 reference_wrapper 的变体，解包后才是元素值
            RTTRVariant element = view.get_value(i).extract_wrapped_value();
            const String elementLabel = "[" + std::to_string(i) + "]";

            // 元素沿用宿主属性的提示，标在容器上的取值范围因此会逐元素生效
            if (drawValue(elementLabel, elementType, element, depth + 1, hints)
                && view.set_value(i, element))
            {
                changed = true;
            }

            if (resizable)
            {
                ImGui::SameLine();

                if (ImGui::SmallButton("-"))
                {
                    pendingErase = i;
                }
            }

            ImGui::PopID();
        }

        if (resizable && ImGui::SmallButton("+"))
        {
            // 用 set_size 追加而不是 insert 一个反射构造出来的元素：set_size 由容器
            // 自己默认构造元素，不要求元素类型向 RTTR 注册过默认构造函数
            if (view.set_size(std::min(view.get_size() + 1, kMaxContainerSize)))
            {
                changed = true;
            }
        }

        if (pendingErase != kNoIndex)
        {
            view.erase(view.begin() + static_cast<int32_t>(pendingErase));
            changed = true;
        }

        ImGui::TreePop();

        return changed;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawAssetReference(const String &label, Meta::Type assetType,
        RTTRVariant &value)
    {
        const UUID current = value.get_value<UUID>();
        const TArray<AssetNode *> roots = assetRoots();

        String text;

        if (current == UUID::INVALID)
        {
            text = "None";
        }
        else
        {
            for (auto root : roots)
            {
                text = findAssetName(root, current);

                if (!text.empty())
                {
                    break;
                }
            }

            // 引用的资产已不在工程里时保留 UUID，便于排查是漏拷资源还是引用写错
            if (text.empty())
            {
                text = "Missing (" + current.toString() + ")";
            }
        }

        bool changed = false;

        ImGui::PushID(label.c_str());

        // 引用字段用按钮承载，点击弹出资产选择器。按钮宽度对齐常规控件、属性名画在
        // 右侧，这样与其它属性的排布保持一致
        if (ImGui::Button(text.c_str(), ImVec2(ImGui::CalcItemWidth(), 0.0f)))
        {
            sAssetCandidates.clear();

            for (auto root : roots)
            {
                collectAssets(root, assetType, sAssetCandidates);
            }

            std::sort(sAssetCandidates.begin(), sAssetCandidates.end(),
                [](const AssetCandidate &lhs, const AssetCandidate &rhs)
                {
                    return lhs.name < rhs.name;
                });

            sAssetFilter.Clear();

            ImGui::OpenPopup(kAssetPickerPopup);
        }

        UUID dropped;

        if (acceptAssetDrop(dropped))
        {
            value = dropped;
            changed = true;
        }

        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::TextUnformatted(displayText(label).c_str());

        if (ImGui::BeginPopup(kAssetPickerPopup))
        {
            // 弹出首帧把键盘焦点放到搜索框上，省去一次点击
            if (ImGui::IsWindowAppearing())
            {
                ImGui::SetKeyboardFocusHere();
            }

            sAssetFilter.Draw("##AssetFilter");

            const float height
                = ImGui::GetTextLineHeightWithSpacing() * kAssetVisibleRows;

            // BeginChild 与 EndChild 必须无条件配对，不能按返回值决定
            ImGui::BeginChild("##AssetList", ImVec2(0.0f, height));

            if (ImGui::Selectable("None", current == UUID::INVALID))
            {
                value = UUID::INVALID;
                changed = true;

                ImGui::CloseCurrentPopup();
            }

            int32_t index = 0;

            for (const auto &candidate : sAssetCandidates)
            {
                if (!sAssetFilter.PassFilter(candidate.name.c_str()))
                {
                    continue;
                }

                // 不同目录下允许存在同名资产，用下标隔离控件 ID
                ImGui::PushID(index++);

                if (ImGui::Selectable(candidate.name.c_str(),
                    candidate.uuid == current))
                {
                    value = candidate.uuid;
                    changed = true;

                    ImGui::CloseCurrentPopup();
                }

                ImGui::PopID();
            }

            ImGui::EndChild();
            ImGui::EndPopup();
        }

        ImGui::PopID();

        return changed;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawObjectReference(const String &label, const RTTRType &type,
        RTTRVariant &value)
    {
        // 只有指向 Object 派生类的指针才是对象引用。其余指针（如 Buffer 里的裸数据
        // 指针）没有可展示的身份，也不该被说成「空引用」，只标出类型
        const RTTRType pointee = type.is_wrapper()
            ? type.get_wrapped_type().get_raw_type() : type.get_raw_type();

        if (!pointee.is_derived_from(RTTRType::get<Object>()))
        {
            drawReadOnlyText(label, stripNamespace(type.get_name().to_string()));
            return false;
        }

        Object *referenced = nullptr;

        if (type.is_wrapper())
        {
            // 判空沿用序列化层验证过的做法：SmartPtr 的 wrapper_mapper::convert 只在
            // 源非空且类型匹配时置成功位，能转成非空 SmartPtr<Object> 即代表引用非空。
            // 不要改成 extract_wrapped_value() 再转 void*，非空 SmartPtr 解包出的裸
            // 指针并不总能转成 void*，会把非空引用误判成空（见 T3DRttrArchive 里
            // isNullValue 的说明）
            RTTRVariant probe = value;

            if (probe.convert(RTTRType::get<SmartPtr<Object>>()))
            {
                referenced = probe.get_value<SmartPtr<Object>>();
            }
        }
        else
        {
            bool ok = false;
            referenced = value.convert<Object *>(&ok);

            if (!ok)
            {
                referenced = nullptr;
            }
        }

        // 对象引用指向运行期对象，没有可枚举的候选集合，因此不像资产引用那样提供
        // 选择器；赋值要靠从层级 / project 面板拖入，而编辑器目前还没有实现拖拽源，
        // 所以这里先只做展示
        if (referenced == nullptr)
        {
            // 空引用时把声明类型带出来，便于知道这里该放什么
            drawReadOnlyText(label,
                "None (" + stripNamespace(type.get_name().to_string()) + ")");
        }
        else
        {
            // 显示运行期派生类型，多态引用下比声明类型更有信息量
            drawReadOnlyText(label,
                stripNamespace(RTTRType::get(*referenced).get_name().to_string()));
        }

        return false;
    }

    //--------------------------------------------------------------------------

    template <typename T>
    bool ImPropertyDrawer::drawScalar(const String &label, ImGuiDataType dataType,
        RTTRVariant &value, float speed, const UIHints &hints)
    {
        T scalar = value.get_value<T>();
        bool edited = false;

        if (hints.hasRange)
        {
            const T low = static_cast<T>(hints.rangeMin);
            const T high = static_cast<T>(hints.rangeMax);

            // 滑条能直接看出当前值在量程里的位置。AlwaysClamp 是为了让 ctrl + 点击
            // 手输的值也落在量程内，否则滑条能显示出越界值
            edited = ImGui::SliderScalar(label.c_str(), dataType, &scalar, &low, &high,
                nullptr, ImGuiSliderFlags_AlwaysClamp);
        }
        else
        {
            edited = ImGui::DragScalar(label.c_str(), dataType, &scalar, speed);
        }

        if (edited)
        {
            value = scalar;
        }

        return edited;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawAngle(const String &label, float &degrees,
        const UIHints &hints)
    {
        if (hints.hasRange)
        {
            // 量程按度解释，与控件显示和编辑的单位一致
            return ImGui::SliderFloat(label.c_str(), &degrees,
                static_cast<float>(hints.rangeMin),
                static_cast<float>(hints.rangeMax), kAngleFormat,
                ImGuiSliderFlags_AlwaysClamp);
        }

        return ImGui::DragFloat(label.c_str(), &degrees, kAngleDragSpeed,
            0.0f, 0.0f, kAngleFormat);
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawArithmetic(const String &label, const RTTRType &type,
        RTTRVariant &value, const UIHints &hints)
    {
        if (type == RTTRType::get<float32_t>())
        {
            return drawScalar<float32_t>(label, ImGuiDataType_Float, value, kDragSpeed, hints);
        }

        if (type == RTTRType::get<float64_t>())
        {
            return drawScalar<float64_t>(label, ImGuiDataType_Double, value, kDragSpeed, hints);
        }

        if (type == RTTRType::get<int8_t>())
        {
            return drawScalar<int8_t>(label, ImGuiDataType_S8, value, 1.0f, hints);
        }

        if (type == RTTRType::get<uint8_t>())
        {
            return drawScalar<uint8_t>(label, ImGuiDataType_U8, value, 1.0f, hints);
        }

        if (type == RTTRType::get<int16_t>())
        {
            return drawScalar<int16_t>(label, ImGuiDataType_S16, value, 1.0f, hints);
        }

        if (type == RTTRType::get<uint16_t>())
        {
            return drawScalar<uint16_t>(label, ImGuiDataType_U16, value, 1.0f, hints);
        }

        if (type == RTTRType::get<int32_t>())
        {
            return drawScalar<int32_t>(label, ImGuiDataType_S32, value, 1.0f, hints);
        }

        if (type == RTTRType::get<uint32_t>())
        {
            return drawScalar<uint32_t>(label, ImGuiDataType_U32, value, 1.0f, hints);
        }

        if (type == RTTRType::get<int64_t>())
        {
            return drawScalar<int64_t>(label, ImGuiDataType_S64, value, 1.0f, hints);
        }

        if (type == RTTRType::get<uint64_t>())
        {
            return drawScalar<uint64_t>(label, ImGuiDataType_U64, value, 1.0f, hints);
        }

        bool ok = false;
        const String text = value.to_string(&ok);
        drawReadOnlyText(label, ok ? text : type.get_name().to_string());

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawEnumeration(const String &label, const RTTRType &type, RTTRVariant &value)
    {
        const rttr::enumeration enumeration = type.get_enumeration();

        if (!enumeration.is_valid())
        {
            drawReadOnlyText(label, type.get_name().to_string());
            return false;
        }

        bool ok = false;
        const String current = value.to_string(&ok);
        bool changed = false;

        if (ImGui::BeginCombo(label.c_str(), ok ? current.c_str() : ""))
        {
            for (const auto &name : enumeration.get_names())
            {
                const String item = name.to_string();
                const bool selected = (item == current);

                if (ImGui::Selectable(item.c_str(), selected) && !selected)
                {
                    value = enumeration.name_to_value(name);
                    changed = true;
                }

                if (selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawString(const String &label, RTTRVariant &value)
    {
        char buffer[kTextBufferSize] = { 0 };
        copyToBuffer(buffer, kTextBufferSize, value.to_string());

        if (ImGui::InputText(label.c_str(), buffer, kTextBufferSize))
        {
            value = String(buffer);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawVector2(const String &label, RTTRVariant &value)
    {
        const Vector2 vector = value.get_value<Vector2>();
        float components[2] =
        {
            static_cast<float>(vector.x()),
            static_cast<float>(vector.y())
        };

        if (ImGui::DragFloat2(label.c_str(), components, kDragSpeed))
        {
            value = Vector2(static_cast<Real>(components[0]),
                static_cast<Real>(components[1]));
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawVector3(const String &label, RTTRVariant &value)
    {
        const Vector3 vector = value.get_value<Vector3>();
        float components[3] =
        {
            static_cast<float>(vector.x()),
            static_cast<float>(vector.y()),
            static_cast<float>(vector.z())
        };

        if (ImGui::DragFloat3(label.c_str(), components, kDragSpeed))
        {
            value = Vector3(static_cast<Real>(components[0]),
                static_cast<Real>(components[1]),
                static_cast<Real>(components[2]));
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawVector4(const String &label, RTTRVariant &value)
    {
        const Vector4 vector = value.get_value<Vector4>();
        float components[4] =
        {
            static_cast<float>(vector.x()),
            static_cast<float>(vector.y()),
            static_cast<float>(vector.z()),
            static_cast<float>(vector.w())
        };

        if (ImGui::DragFloat4(label.c_str(), components, kDragSpeed))
        {
            value = Vector4(static_cast<Real>(components[0]),
                static_cast<Real>(components[1]),
                static_cast<Real>(components[2]),
                static_cast<Real>(components[3]));
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawQuaternion(const String &label, RTTRVariant &value)
    {
        const Quaternion quaternion = value.get_value<Quaternion>();

        // 四元数对用户不直观，UI 上统一用欧拉角（度）编辑
        Radian pitch;
        Radian yaw;
        Radian roll;
        quaternion.toEulerAnglesXYZ(pitch, yaw, roll);

        float angles[3] =
        {
            static_cast<float>(pitch.valueDegrees()),
            static_cast<float>(yaw.valueDegrees()),
            static_cast<float>(roll.valueDegrees())
        };

        const ImGuiID id = ImGui::GetID(label.c_str());

        if (sEulerCache.id == id)
        {
            ::memcpy(angles, sEulerCache.angles, sizeof(angles));
        }

        bool changed = false;

        if (ImGui::DragFloat3(label.c_str(), angles, kAngleDragSpeed,
            0.0f, 0.0f, kAngleFormat))
        {
            Quaternion result;
            result.fromEulerAnglesXYZ(
                Radian(Degree(static_cast<Real>(angles[0])).valueRadians()),
                Radian(Degree(static_cast<Real>(angles[1])).valueRadians()),
                Radian(Degree(static_cast<Real>(angles[2])).valueRadians()));

            value = result;
            changed = true;
        }

        if (ImGui::IsItemActive())
        {
            sEulerCache.id = id;
            ::memcpy(sEulerCache.angles, angles, sizeof(angles));
        }
        else if (sEulerCache.id == id)
        {
            sEulerCache.id = 0;
        }

        return changed;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawRadian(const String &label, RTTRVariant &value,
        const UIHints &hints)
    {
        const Radian radian = value.get_value<Radian>();

        // 弧度对用户不直观，UI 上统一按角度编辑
        float degrees = static_cast<float>(radian.valueDegrees());

        if (drawAngle(label, degrees, hints))
        {
            value = Radian(Degree(static_cast<Real>(degrees)).valueRadians());
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawDegree(const String &label, RTTRVariant &value,
        const UIHints &hints)
    {
        const Degree degree = value.get_value<Degree>();
        float degrees = static_cast<float>(degree.valueDegrees());

        if (drawAngle(label, degrees, hints))
        {
            value = Degree(static_cast<Real>(degrees));
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawColorRGB(const String &label, RTTRVariant &value)
    {
        const ColorRGB color = value.get_value<ColorRGB>();
        float components[3] = { color.red(), color.green(), color.blue() };

        if (ImGui::ColorEdit3(label.c_str(), components))
        {
            value = ColorRGB(components[0], components[1], components[2]);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    bool ImPropertyDrawer::drawColorRGBA(const String &label, RTTRVariant &value)
    {
        const ColorRGBA color = value.get_value<ColorRGBA>();
        float components[4] =
        {
            color.red(), color.green(), color.blue(), color.alpha()
        };

        if (ImGui::ColorEdit4(label.c_str(), components))
        {
            value = ColorRGBA(components[0], components[1],
                components[2], components[3]);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------

    void ImPropertyDrawer::drawReadOnlyText(const String &label, const String &text)
    {
        ImGui::BeginDisabled();
        ImGui::LabelText(label.c_str(), "%s", text.c_str());
        ImGui::EndDisabled();
    }

    //--------------------------------------------------------------------------
    NS_END
}
