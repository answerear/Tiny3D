/*******************************************************************************
 * {ProjectName} —— 示例脚本
 ******************************************************************************/

#pragma once


#include "GamePluginPrerequisites.h"

#include "Component/T3DBehaviour.h"
#include "Component/T3DTransform3D.h"


/**
 * \brief 让挂载的物体绕自身 Y 轴匀速旋转的示例脚本
 * \remarks
 *   照着这个类的结构写你自己的脚本就行，要点有三个：
 *   1. 从 Behaviour 派生，加 TRTTI_ENABLE(Tiny3D::Behaviour) 与 TRTTI_FRIEND；
 *   2. 在对应的 .cpp 里写 RTTR_REGISTRATION 把类型和属性注册进去，编辑器靠它
 *      认出这个脚本、在 Inspector 里显示属性、以及把属性存进场景文件；
 *   3. 逻辑写在 onUpdate 等生命周期回调里，它们只在 Play 时被调度。
 *
 *   注意业务代码只能用引擎的 Runtime API。Scene::getEditorCamera、EditorScene、
 *   PrefabUtility 这类编辑器专有接口在 Editor 变体下能编过，但发布时要编的
 *   Runtime 变体没有它们。可以用菜单里的 Validate Runtime Build 随时确认。
 */
class ExampleBehaviour : public Tiny3D::Behaviour
{
    TRTTI_ENABLE(Tiny3D::Behaviour)
    TRTTI_FRIEND

public:
    ~ExampleBehaviour() override = default;

    /// 每秒转过的角度
    void setAngularSpeedDeg(Tiny3D::Real degPerSec) { mAngularSpeedDeg = degPerSec; }
    Tiny3D::Real getAngularSpeedDeg() const { return mAngularSpeedDeg; }

protected:
    ExampleBehaviour() = default;
    explicit ExampleBehaviour(const Tiny3D::UUID &uuid);

    /// 实例化完成、同物体上其它组件都已就位时调用一次
    void onAwake() override;

    /// 每帧调用
    void onUpdate() override;

private:
    Tiny3D::Real mAngularSpeedDeg {90.0f};

    // 不需要序列化的运行期缓存，所以不注册成 RTTR 属性
    Tiny3D::Transform3DPtr mTransform {nullptr};
};
