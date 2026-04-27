#pragma once
#include "core/UIComponent.h"

class TestVerticalSlider : public UIComponent
{
public:
    TestVerticalSlider();
    void Render() override;

private:
    float joint_position[7];
    float joint_velocity[7];
    float joint_torque[7];
};