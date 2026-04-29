#include "ControlModeView.h"
#include <imgui.h>
#include "components/layout/EmanipCustomWidgets.h"

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
        { { 0.5f, 0.5f, 0.5f, 1.0f }, "CONTROL MODE : UNKNOWN"          },  // 0 - Unknown
        { { 1.0f, 1.0f, 1.0f, 1.0f }, "CONTROL MODE : STANDBY"        },  // 1 - Standby
        { { 0.0f, 1.0f, 1.0f, 1.0f }, "CONTROL MODE : ACTIVE"     },  // 2 - Active
    };
    
    int stateIndex = static_cast<int>(state);
    if (stateIndex < 0 || stateIndex >= static_cast<int>(std::size(styles)))
        stateIndex = 0;
    
    const auto& s = styles[stateIndex];
    ImGui::TextColored(s.colour, "%s", s.label);
}

void ControlModeView::DrawButtons()
{
    if (EmanipCustomWidgets::PushButton("Set Standby", {120, 50},
        ImVec4(1.f, 0.671f, 0.f, 1.f))) // orange
    {
        controller_->SetStandby();
    }

    if (EmanipCustomWidgets::PushButton("Set Active", {120, 50},
        ImVec4(0.0f, 0.8f, 0.0f, 1.0f))) // green
    {
        controller_->SetActive();
    }
}