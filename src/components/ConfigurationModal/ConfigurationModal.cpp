#include "components/ConfigurationModal/ConfigurationModal.h"
#include <imgui.h>
#include <cstring>
#include <iostream>

ConfigurationModalComponent::ConfigurationModalComponent()
    : port_(8765)
    , show_configuration_modal_(false)
    , show_shutdown_modal_(false)
    , pending_shutdown_confirm_(false)
{
    std::strncpy(ip_buf_, "172.19.171.48", sizeof(ip_buf_));
}

void ConfigurationModalComponent::Render()
{
    ImGui::Begin("Configuration###ConfigurationModal");
    DrawPanel(window);
    ImGui::End();
}

void ConfigurationModalComponent::DrawPanel(GLFWwindow* win)
{
    if (ImGui::Button("Configuration", { -1, 30 }))
        show_configuration_modal_ = true;

    if (show_configuration_modal_)
        ImGui::OpenPopup("Configuration###ConfigModal");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, { 0.5f, 0.5f });
    ImGui::SetNextWindowSize({ 400, 260 }, ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Configuration###ConfigModal", &show_configuration_modal_,
                               ImGuiWindowFlags_NoResize))
    {
        if (ImGui::BeginTabBar("ConfigTabs"))
        {
            if (ImGui::BeginTabItem("Network"))
            {
                DrawNetworkTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Options"))
            {
                DrawOptionsTab();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndPopup();
    }

    // Shutdown confirm lives outside the config modal so the popup stack is clean
    if (pending_shutdown_confirm_)
    {
        std::cout << "[Shutdown] pending_shutdown_confirm_ is true, opening popup" << std::endl;
        ImGui::OpenPopup("Confirm Shutdown");
        pending_shutdown_confirm_ = false;
    }

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, { 0.5f, 0.5f });
    if (ImGui::BeginPopupModal("Confirm Shutdown", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to shut down?");
        ImGui::Separator();

        if (ImGui::Button("Yes", { 120, 0 }))
        {
            std::cout << "[Shutdown] Yes clicked, window ptr = " << win << std::endl;
            if (win) 
            {
                std::cout << "[Shutdown] Calling glfwSetWindowShouldClose" << std::endl;
                glfwSetWindowShouldClose(win, GLFW_TRUE);
            }
            else
            {
                std::cout << "[Shutdown] window is NULL - this is the problem" << std::endl;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No", { 120, 0 }))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ConfigurationModalComponent::DrawNetworkTab()
{
    ImGui::Spacing();
    ImGui::Text("Server IP");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##ip", ip_buf_, sizeof(ip_buf_));

    ImGui::Spacing();
    ImGui::Text("Port");
    ImGui::SetNextItemWidth(-1);
    ImGui::InputInt("##port", &port_);

    ImGui::Spacing();
    if (ImGui::Button("Save", { -1, 30 }))
    {
        show_configuration_modal_ = false;
        ImGui::CloseCurrentPopup();
    }
}

void ConfigurationModalComponent::DrawOptionsTab()
{
    ImGui::Spacing();
    if (ImGui::Button("Shutdown Application", { -1, 30 }))
    {
        std::cout << "[Shutdown] Shutdown button clicked" << std::endl;
        show_configuration_modal_ = false;
        ImGui::CloseCurrentPopup();
        pending_shutdown_confirm_ = true;
    }
}