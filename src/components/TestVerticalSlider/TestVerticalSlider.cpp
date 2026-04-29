#include "components/TestVerticalSlider/TestVerticalSlider.h"
#include "components/JointPositionController/JointPositionController.h"
#include <imgui.h>
#include <string>
#include <algorithm>

TestVerticalSlider::TestVerticalSlider()
: joint_position{0, 0, 0, 0, 0, 0, 0, 0},
  joint_velocity{24.0f, 24.0f, 24.0f, 24.0f, 24.0f, 24.0f, 24.0f, 24.0f},
  joint_torque{50.0f, 50.0f, 50.0f, 50.0f, 50.0f, 50.0f, 50.0f, 50.0f}
{
}

const int JOINT_ID_START_VALUE = 1;

void TestVerticalSlider::SetController(std::shared_ptr<JointPositionController> controller)
{
    controller_ = std::move(controller);
}

void TestVerticalSlider::Render()
{
    ImGui::Begin("Joint Control");

    if (controller_)
    {
        if (ImGui::Button("Update to arm position"))
        {
            for (int i = JOINT_ID_START_VALUE; i < NUM_JOINTS; i++)
            {
                joint_position[i] = controller_->GetJointPosition(i);
                joint_velocity[i] = controller_->GetJointVelocity(i);
                joint_torque[i]   = controller_->GetJointTorque(i);
            }
        }
    }

    ImGuiStyle& style      = ImGui::GetStyle();
    float originalGrabSize = style.GrabMinSize;
    style.GrabMinSize      = 35.0f;

    // Calculate the height to match slider column content:
    // label + slider + pos text + vel row + torque row + spacing
    const float sliderHeight   = 300.0f;
    const float rowHeight      = ImGui::GetFrameHeight();
    const float columnHeight   = rowHeight         // J label
                               + sliderHeight      // slider
                               + rowHeight         // pos text
                               + rowHeight         // vel input row
                               + rowHeight         // torque input row
                               + style.ItemSpacing.y * 4;

    int totalColumns = (NUM_JOINTS - JOINT_ID_START_VALUE) + 1;
    if (ImGui::BeginTable("JointSliders", totalColumns, ImGuiTableFlags_SizingFixedFit))
    {
        for (int i = JOINT_ID_START_VALUE; i < NUM_JOINTS; i++)
            ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 100.0f);

        // Joint columns
        for (int i = JOINT_ID_START_VALUE; i < NUM_JOINTS; i++)
        {
            ImGui::TableNextColumn();

            std::string pos_id    = "##joint_" + std::to_string(i) + "_pos";
            std::string vel_id    = "##joint_" + std::to_string(i) + "_vel";
            std::string torque_id = "##joint_" + std::to_string(i) + "_torque";

            ImGui::Text("J%d", i);

            bool posChanged = ImGui::VSliderFloat(
                pos_id.c_str(), ImVec2(100, sliderHeight), &joint_position[i],
                (i == 7) ? 0.0f : -90.0f, 90.0f
            );
            ImGui::Text("%.1f deg", joint_position[i]);

            // Velocity — input + label side by side, no step buttons
            ImGui::SetNextItemWidth(60.0f);
            ImGui::InputFloat(vel_id.c_str(), &joint_velocity[i], 0.0f, 0.0f, "%.1f");
            ImGui::SameLine(); ImGui::Text("deg/s");

            // Torque — input + label side by side, no step buttons
            ImGui::SetNextItemWidth(60.0f);
            ImGui::InputFloat(torque_id.c_str(), &joint_torque[i], 0.0f, 0.0f, "%.2f");
            ImGui::SameLine(); ImGui::Text("%%");

            joint_torque[i] = std::clamp(joint_torque[i], 0.0f, 100.0f);

            if (controller_ && posChanged)
                controller_->SetJointPosition(i, joint_position[i], joint_velocity[i], joint_torque[i]);
        }

        // Jaw column
        ImGui::TableNextColumn();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));
        if (ImGui::BeginChild("JawControlPanel", ImVec2(150, columnHeight), true))
        {
            ImGui::Text("Jaw");
            ImGui::Spacing();

            // Velocity — input + label side by side, no step buttons
            ImGui::SetNextItemWidth(80.0f);
            ImGui::InputFloat("##jaw_vel",    &jaw_velocity_, 0.0f, 0.0f, "%.1f");
            ImGui::SameLine(); ImGui::Text("vel");

            // Torque — input + label side by side, no step buttons
            ImGui::SetNextItemWidth(80.0f);
            ImGui::InputFloat("##jaw_torque", &jaw_torque_,   0.0f, 0.0f, "%.1f");
            ImGui::SameLine(); ImGui::Text("torque");

            jaw_velocity_ = std::clamp(jaw_velocity_, 0.0f, 100.0f);
            jaw_torque_   = std::clamp(jaw_torque_,   0.0f, 100.0f);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Close",   { 120, 30 }) && controller_)
                controller_->SendJawCommand(1, jaw_velocity_, jaw_torque_);

            if (ImGui::Button("Release", { 120, 30 }) && controller_)
                controller_->SendJawCommand(2, jaw_velocity_, jaw_torque_);

            if (ImGui::Button("Open",    { 120, 30 }) && controller_)
                controller_->SendJawCommand(3, jaw_velocity_, jaw_torque_);

            ImGui::EndChild();
        }
        ImGui::PopStyleColor();

        ImGui::EndTable();
    }

    style.GrabMinSize = originalGrabSize;
    ImGui::End();
}

// void TestVerticalSlider::Render()
// {
//     ImGui::Begin("Joint Control");

//     if (controller_)
//     {
//         if (ImGui::Button("Update to arm position"))
//         {
//             for (int i = JOINT_ID_START_VALUE; i < NUM_JOINTS; i++)
//             {
//                 joint_position[i] = controller_->GetJointPosition(i);
//                 joint_velocity[i] = controller_->GetJointVelocity(i);
//                 joint_torque[i]   = controller_->GetJointTorque(i);
//             }
//         }
//     }

//     ImGuiStyle& style       = ImGui::GetStyle();
//     float originalGrabSize  = style.GrabMinSize;
//     style.GrabMinSize       = 25.0f;

//     if (ImGui::BeginTable("JointSliders", NUM_JOINTS, ImGuiTableFlags_SizingFixedFit))
//     {
//         for (int i = JOINT_ID_START_VALUE; i < NUM_JOINTS; i++)
//             ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 140.0f);

//         for (int i = JOINT_ID_START_VALUE; i < NUM_JOINTS; i++)
//         {
//             ImGui::TableNextColumn();

//             std::string pos_id    = "##joint_" + std::to_string(i) + "_pos";
//             std::string vel_id    = "##joint_" + std::to_string(i) + "_vel";
//             std::string torque_id = "##joint_" + std::to_string(i) + "_torque";

//             ImGui::Text("J%d", i);

//             bool posChanged = ImGui::VSliderFloat(
//                 pos_id.c_str(), ImVec2(60, 250), &joint_position[i],
//                 (i == 7) ? 0.0f : -90.0f,
//                 90.0f
//             );
//             ImGui::Text("%.1f deg", joint_position[i]);

//             ImGui::SetNextItemWidth(120.0f);
//             bool velChanged    = ImGui::InputFloat(vel_id.c_str(), &joint_velocity[i], 1.0f, 5.0f, "%.1f");
//             ImGui::Text("deg/s");

//             ImGui::SetNextItemWidth(120.0f);
//             bool torqueChanged = ImGui::InputFloat(torque_id.c_str(), &joint_torque[i], 1.0f, 5.0f, "%.2f");
//             ImGui::Text("%%");

//             joint_torque[i] = std::clamp(joint_torque[i], 0.0f, 100.0f);

//             if (controller_ && posChanged)
//             {
//                 controller_->SetJointPosition(i, joint_position[i], joint_velocity[i], joint_torque[i]);
//             }
//         }
//         ImGui::EndTable();
//     }

//     style.GrabMinSize = originalGrabSize;

//     ImGui::Separator();
//     ImGui::Text("Jaw Control");
//     ImGui::Spacing();

//     ImGui::SetNextItemWidth(120.0f);
//     ImGui::InputFloat("##jaw_vel",    &jaw_velocity_, 1.0f, 5.0f, "%.1f");
//     ImGui::SameLine(); ImGui::Text("vel");

//     ImGui::SetNextItemWidth(120.0f);
//     ImGui::InputFloat("##jaw_torque", &jaw_torque_,   1.0f, 5.0f, "%.1f");
//     ImGui::SameLine(); ImGui::Text("torque");

//     jaw_velocity_ = std::clamp(jaw_velocity_, 0.0f, 100.0f);
//     jaw_torque_   = std::clamp(jaw_torque_,   0.0f, 100.0f);

//     ImGui::Spacing();

//     if (ImGui::Button("Close",   { 80, 30 }) && controller_)
//         controller_->SendJawCommand(1, jaw_velocity_, jaw_torque_);

//     ImGui::SameLine();

//     if (ImGui::Button("Release", { 80, 30 }) && controller_)
//         controller_->SendJawCommand(2, jaw_velocity_, jaw_torque_);

//     ImGui::SameLine();

//     if (ImGui::Button("Open",    { 80, 30 }) && controller_)
//         controller_->SendJawCommand(3, jaw_velocity_, jaw_torque_);

//     ImGui::End();
// }
