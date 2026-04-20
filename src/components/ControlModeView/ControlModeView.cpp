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

    ImGui::Separator();
    DrawButtons();

    ImGui::End();
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