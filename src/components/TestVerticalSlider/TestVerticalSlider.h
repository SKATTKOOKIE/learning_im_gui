#pragma once
#include "core/UIComponent.h"
#include <memory>

class JointPositionController;

class TestVerticalSlider : public UIComponent
{
public:
    TestVerticalSlider();
    void Render() override;
    
    void SetController(std::shared_ptr<JointPositionController> controller);

private:
    static const int NUM_JOINTS = 8;
    float joint_position[NUM_JOINTS];
    float joint_velocity[NUM_JOINTS];
    float joint_torque[NUM_JOINTS];

    float jaw_velocity_{24.0f};
    float jaw_torque_{50.0f};
    
    std::shared_ptr<JointPositionController> controller_;
};