#include "ControlModeView.h"
#include <imgui.h>

ControlModeView::ControlModeView(std::shared_ptr<ControlModeController> controller)
    : controller_(std::move(controller))
{}

void ControlModeView::Open()  { isOpen_ = true;  }
void ControlModeView::Close() { isOpen_ = false; }

void ControlModeView::Draw()
{
    ImGui::SetNextWindowSize({ 320, 160 }, ImGuiCond_FirstUseEver);
    if (!isOpen_ || !ImGui::Begin("ControlMode###ControlModeView", &isOpen_,
        ImGuiWindowFlags_NoCollapse))
        return;

    DrawStateIndicator();
    ImGui::Separator();
    DrawButtons();

    ImGui::End();
}

void ControlModeView::DrawStateIndicator() const
{
    const auto state = controller_->GetState();
    
    // Map ConnectionState enum to display styles
    struct StateStyle { ImVec4 colour; const char* label; };
    
    static constexpr StateStyle styles[] = {
        { { 0.5f, 0.5f, 0.5f, 1.0f }, "● UNKNOWN"          },  // 0 - Unknown
        { { 0.0f, 1.0f, 0.0f, 1.0f }, "● STANDBY"        },  // 1 - Standby
        { { 0.5f, 0.5f, 0.5f, 1.0f }, "● ACTIVE"     },  // 2 - Active
    };
    
    int stateIndex = static_cast<int>(state);
    if (stateIndex < 0 || stateIndex >= static_cast<int>(std::size(styles)))
        stateIndex = 0;
    
    const auto& s = styles[stateIndex];
    ImGui::TextColored(s.colour, "%s", s.label);
}

void ControlModeView::DrawButtons()
{
    {
        if (ImGui::Button("Set Standby", { 120, 30 }))
            controller_->SetStandby();
        
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            ImGui::SetTooltip("Send set standby command to EMANIP");
    }
    
    ImGui::SameLine();
    
    {
        if (ImGui::Button("Set Active", { 120, 30 }))
            controller_->SetActive();
        
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            ImGui::SetTooltip("Send set active command to EMANIP");
    }
}