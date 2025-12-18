#include "components/test/TestComponent.h"
#include <imgui.h>
#include <GLFW/glfw3.h>

TestComponent::TestComponent()
    : show_example_text(false), show_shutdown_modal(false)
{
}

void TestComponent::Render()
{
    ImGui::Begin("This is a test window for ImGui");

    ImGui::Checkbox("Unhide the text", &show_example_text);

    if (show_example_text)
    {
        ImGui::Text("I am the hidden text!");
    }

    // Shutdown button
    if (ImGui::Button("Shutdown"))
    {
        show_shutdown_modal = true;
    }

    // Confirmation modal
    if (show_shutdown_modal)
    {
        ImGui::OpenPopup("Confirm Shutdown");
    }

    if (ImGui::BeginPopupModal("Confirm Shutdown", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to shut down the application?");
        ImGui::Separator();

        if (ImGui::Button("Yes", ImVec2(120, 0)))
        {
            // Use the window pointer from the base class
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("No", ImVec2(120, 0)))
        {
            show_shutdown_modal = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}