#include "components/TestVerticalSlider/TestVerticalSlider.h"
#include "components/JointPositionController/JointPositionController.h"
#include <imgui.h>
#include <string>

TestVerticalSlider::TestVerticalSlider()
    : joint_position{0, 0, 0, 0, 0, 0, 0},
      joint_velocity{24.0f, 24.0f, 24.0f, 24.0f, 24.0f, 24.0f, 24.0f},
      joint_torque{50.0f, 50.0f, 50.0f, 50.0f, 50.0f, 50.0f, 50.0f}
{
}

void TestVerticalSlider::SetController(std::shared_ptr<JointPositionController> controller)
{
    controller_ = std::move(controller);
}

void TestVerticalSlider::Render()
{
    ImGui::Begin("Joint Control");
    
    if (ImGui::BeginTable("JointSliders", NUM_JOINTS, ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        
        for (int i = 0; i < NUM_JOINTS; i++)
        {
            ImGui::TableNextColumn();
            
            // Create unique ID strings for each joint
            std::string pos_id = "##joint_" + std::to_string(i) + "_position_slider";
            std::string vel_id = "##joint_" + std::to_string(i) + "_velocity_input";
            std::string torque_id = "##joint_" + std::to_string(i) + "_torque_input";
            
            // Position slider
            bool positionChanged = ImGui::VSliderFloat(pos_id.c_str(), ImVec2(60, 120), &joint_position[i], -90.0f, 90.0f);
            
            // Position value text (1 decimal place)
            ImGui::Text("%.1f°", joint_position[i]);
            
            // Velocity input (deg/s, default 24.0, 1 decimal place)
            ImGui::SetNextItemWidth(120.0f);
            bool velocityChanged = ImGui::InputFloat(vel_id.c_str(), &joint_velocity[i], 1.0f, 5.0f, "%.1f");
            ImGui::Text("deg/s");
            
            // Torque input (percentage 0-100, scales to 0-100.00)
            ImGui::SetNextItemWidth(120.0f);
            bool torqueChanged = ImGui::InputFloat(torque_id.c_str(), &joint_torque[i], 1.0f, 5.0f, "%.2f");
            ImGui::Text("%%");
            
            // Clamp torque to 0-100% range
            if (joint_torque[i] < 0.0f) joint_torque[i] = 0.0f;
            if (joint_torque[i] > 100.0f) joint_torque[i] = 100.0f;
            
            // Send command to controller if any value changed
            if (controller_ && (positionChanged || velocityChanged || torqueChanged))
            {
                controller_->SetJointPosition(i, joint_position[i], joint_velocity[i], joint_torque[i]);
            }
        }
        ImGui::EndTable();
    }
    
    ImGui::End();
}