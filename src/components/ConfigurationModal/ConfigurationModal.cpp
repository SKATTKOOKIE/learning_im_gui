#include "components/ConfigurationModal/ConfigurationModal.h"
#include <imgui.h>
#include <GLFW/glfw3.h>

ConfigurationModalComponent::ConfigurationModalComponent()
: show_example_text(false), show_configuration_modal(false), show_demo_window(false)
{
}

void ConfigurationModalComponent::Render()
{
    ImGui::Begin("Configuration");
    // Shutdown button
    if (ImGui::Button("Configuration"))
    {
        show_configuration_modal = true;
    }
    if (ImGui::Button("Open Demo"))
    {
        show_demo_window = true;
    }
    // Confirmation modal
    if (show_configuration_modal)
    {
        ImGui::OpenPopup("Confirm Configuration");
    }
    if (ImGui::BeginPopupModal("Confirm Configuration", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Here you can enter your configuration such as IP and Port");
        ImGui::Separator();
        if (ImGui::Button("Save", ImVec2(120, 0)))
        {
            // Add save functionality here
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            show_configuration_modal = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    // Show demo window if requested
    if (show_demo_window)
    {
        ImGui::ShowDemoWindow(&show_demo_window);
    }
    
    ImGui::End();
}