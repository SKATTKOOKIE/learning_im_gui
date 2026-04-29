#include "ConnectionView.h"
#include <imgui.h>
#include "components/layout/EmanipCustomWidgets.h"

ConnectionView::ConnectionView(std::shared_ptr<ConnectionController> controller)
: controller_(std::move(controller))
{}

void ConnectionView::Open() 
{ 
    isOpen_ = true; 
}

void ConnectionView::Close() 
{ 
    isOpen_ = false; 
}

void ConnectionView::Draw()
{
    ImGui::SetNextWindowSize({ 320, 160 }, ImGuiCond_FirstUseEver);
    
    // Add docking flags to properly support docking
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse;
    
    // Check if we're in a docking context and add appropriate flags
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        window_flags |= ImGuiWindowFlags_NoDocking;
    }
    
    if (!isOpen_ || !ImGui::Begin("EMANIPConnection###ConnectionView", &isOpen_, window_flags))
        return;
        
    DrawStateIndicator();
    ImGui::Separator();
    DrawButtons();
    ImGui::End();
}

void ConnectionView::DrawStateIndicator() const
{
    const auto state = controller_->GetState();
    
    // Map ConnectionState enum to display styles
    struct StateStyle { 
        ImVec4 colour; 
        const char* label; 
    };
    
    static constexpr StateStyle styles[] = {
        { { 0.5f, 0.5f, 0.5f, 1.0f }, "CONNECTION STATE : UNKNOWN" },          // 0 - Unknown
        { { 0.0f, 1.0f, 0.0f, 1.0f }, "CONNECTION STATE : CONNECTED" },        // 1 - Connected
        { { 0.5f, 0.5f, 0.5f, 1.0f }, "CONNECTION STATE : DISCONNECTED" },     // 2 - Disconnected
    };
    
    int stateIndex = static_cast<int>(state);
    if (stateIndex < 0 || stateIndex >= static_cast<int>(std::size(styles)))
        stateIndex = 0;
        
    const auto& s = styles[stateIndex];
    ImGui::TextColored(s.colour, "%s", s.label);
}

void ConnectionView::DrawButtons()
{
    if (EmanipCustomWidgets::PushButton("Connect", {120, 50},
        ImVec4(0.0f, 0.8f, 0.0f, 1.0f))) // green
    {
        controller_->Connect();
    }

    if (EmanipCustomWidgets::PushButton("Disconnect", {120, 50},
        ImVec4(1.f, 0.671f, 0.f, 1.f))) // orange
    {
        controller_->Disconnect();
    }
}