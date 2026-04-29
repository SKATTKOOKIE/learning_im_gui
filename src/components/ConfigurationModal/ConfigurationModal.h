#pragma once
#include "core/UIComponent.h"
#include <GLFW/glfw3.h>
#include <functional>
#include <string>

class ConfigurationModalComponent : public UIComponent
{
public:
    using SaveCallback = std::function<void(const std::string& ip, int commandPort, int telemetryPort)>;

    ConfigurationModalComponent();
    void Render() override;
    void DrawPanel(GLFWwindow* win);
    void SetSaveCallback(SaveCallback cb) { saveCallback_ = cb; }

private:
    void DrawNetworkTab();
    void DrawOptionsTab();

    char  ip_buf_[64];
    int   commandPort_;
    int   telemetryPort_;
    bool  show_configuration_modal_;
    bool  show_shutdown_modal_;
    bool  pending_shutdown_confirm_;

    SaveCallback saveCallback_;
};