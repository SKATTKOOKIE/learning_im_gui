#pragma once
#include "core/UIComponent.h"
#include <GLFW/glfw3.h>

class ConfigurationModalComponent : public UIComponent
{
public:
    ConfigurationModalComponent();
    void Render() override;
    void DrawPanel(GLFWwindow* win);

private:
    void DrawNetworkTab();
    void DrawOptionsTab();

    char  ip_buf_[64];
    int   port_;
    bool  show_configuration_modal_;
    bool  show_shutdown_modal_;
    bool  pending_shutdown_confirm_;
};