/*******************************************************************************
 * {ProjectName} —— 示例脚本
 ******************************************************************************/

#include "ExampleBehaviour.h"

#include "Kernel/T3DGameObject.h"
#include "Kernel/T3DTime.h"

#include <rttr/registration>


// 类型注册在 DLL 被加载时自动执行，卸载时 RTTR 会自动把这些条目注销掉，
// 编辑器的热重载就是靠这个特性做到改完代码不用重启的。
RTTR_REGISTRATION
{
    using namespace rttr;
    using namespace Tiny3D;

    registration::class_<ExampleBehaviour>("ExampleBehaviour")
        // 注册成属性的字段才会显示在 Inspector 里，也才会被存进场景文件
        .property("AngularSpeedDeg",
            &ExampleBehaviour::getAngularSpeedDeg,
            &ExampleBehaviour::setAngularSpeedDeg)
        .constructor<>()
        (
            policy::ctor::as_raw_ptr
        )
        .constructor<const Tiny3D::UUID &>()
        (
            policy::ctor::as_raw_ptr
        );

    // 让引擎能把 SmartPtr<ExampleBehaviour> 当作基类指针来处理
    type::register_wrapper_converter_for_base_classes<SmartPtr<ExampleBehaviour>>();

    registration::class_<SmartPtr<ExampleBehaviour>>("Tiny3D::SmartPtr<ExampleBehaviour>")
        .constructor<Object*>()
        (
            policy::ctor::as_object
        )
        .constructor<Object&>()
        (
            policy::ctor::as_object
        );
}


using namespace Tiny3D;


// UUID 必须写全限定名：Windows SDK 的 rpcdce.h 里也有个全局 UUID，
// using namespace Tiny3D 之后两者会撞名
ExampleBehaviour::ExampleBehaviour(const Tiny3D::UUID &uuid)
    : Behaviour(uuid)
{
}

//------------------------------------------------------------------------------

void ExampleBehaviour::onAwake()
{
    mTransform = getGameObject()->getComponent<Transform3D>();
}

//------------------------------------------------------------------------------

void ExampleBehaviour::onUpdate()
{
    if (mTransform == nullptr)
    {
        return;
    }

    // Time::deltaTime() 单位是毫秒
    const Real dtSec = static_cast<Real>(Time::deltaTime()) / static_cast<Real>(1000.0f);
    mTransform->yaw(Degree(mAngularSpeedDeg * dtSec));
}
