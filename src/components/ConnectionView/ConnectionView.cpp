#include "ConnectionView.h"
#include <imgui.h>

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
        { { 0.5f, 0.5f, 0.5f, 1.0f }, "CONNECTTION STATE : UNKNOWN" },          // 0 - Unknown
        { { 0.0f, 1.0f, 0.0f, 1.0f }, "CONNECTTION STATE : CONNECTED" },        // 1 - Connected
        { { 0.5f, 0.5f, 0.5f, 1.0f }, "CONNECTTION STATE : DISCONNECTED" },     // 2 - Disconnected
    };
    
    int stateIndex = static_cast<int>(state);
    if (stateIndex < 0 || stateIndex >= static_cast<int>(std::size(styles)))
        stateIndex = 0;
        
    const auto& s = styles[stateIndex];
    ImGui::TextColored(s.colour, "%s", s.label);
}

void ConnectionView::DrawButtons()
{
    const auto state = controller_->GetState();
    
    // Buttons are always available - they just send their command
    // The server will respond with appropriate status
    
    // Connect button
    {
        if (ImGui::Button("Connect", { 120, 30 }))
            controller_->Connect();
            
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            ImGui::SetTooltip("Send connect command to ROV");
    }
    
    ImGui::SameLine();
    
    // Disconnect button
    {
        if (ImGui::Button("Disconnect", { 120, 30 }))
            controller_->Disconnect();
            
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            ImGui::SetTooltip("Send disconnect command to ROV");
    }
}