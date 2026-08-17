/*******************************************************************************
 * {ProjectName} —— {ClassName}
 ******************************************************************************/

#pragma once


#include "GamePluginPrerequisites.h"

#include "Component/T3DBehaviour.h"


TCLASS()
class {ClassName} : public Tiny3D::Behaviour
{
    TRTTI_ENABLE(Tiny3D::Behaviour)
    TRTTI_FRIEND

public:
    ~{ClassName}() override = default;

protected:
    {ClassName}() = default;
    explicit {ClassName}(const Tiny3D::UUID &uuid);

    void onAwake() override;
    void onEnable() override;
    void onStart() override;
    void onUpdate() override;
    void onLateUpdate() override;
    void onFixedUpdate() override;
    void onDisable() override;
    void onDestroy() override;
};
