#include "LeftColumnWidget.h"
#include <imgui.h>
#include <iostream>

LeftColumnWidget::LeftColumnWidget(
    std::shared_ptr<ConnectionView> connectionView,
    std::shared_ptr<ControlModeView> controlModeView,
    std::shared_ptr<ConfigurationModalComponent> configModal)
    : connectionView_(std::move(connectionView))
    , controlModeView_(std::move(controlModeView))
    , configModal_(std::move(configModal))
{}

void LeftColumnWidget::SetWindow(GLFWwindow* win)
{
    std::cout << "[SetWindow] LeftColumnWidget::SetWindow called, ptr = " << win << std::endl;
    window = win;
    configModal_->SetWindow(win);
    std::cout << "[SetWindow] configModal_ window set" << std::endl;
}
void LeftColumnWidget::Render()
{
    ImGui::Begin("Left Panel###LeftColumnWidget", nullptr,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::BeginChild("ConnectionSection", { 0, 100 }, true))
    {
        connectionView_->DrawStateIndicator();
        ImGui::Separator();
        connectionView_->DrawButtons();
    }
    ImGui::EndChild();

    ImGui::Spacing();

    if (ImGui::BeginChild("ControlModeSection", { 0, 100 }, true))
    {
        controlModeView_->DrawStateIndicator();
        ImGui::Separator();
        controlModeView_->DrawButtons();
    }
    ImGui::EndChild();

    ImGui::Spacing();

    if (ImGui::BeginChild("ConfigSection", { 0, 0 }, true))
    {
        configModal_->DrawPanel(window);
    }
    ImGui::EndChild();

    ImGui::End();
}