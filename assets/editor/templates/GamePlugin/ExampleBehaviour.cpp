/*******************************************************************************
 * {ProjectName} —— 示例脚本
 ******************************************************************************/

#include "ExampleBehaviour.h"

#include "Kernel/T3DGameObject.h"
#include "Kernel/T3DTime.h"


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
