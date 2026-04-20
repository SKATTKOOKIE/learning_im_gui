#pragma once
#include <core/UIComponent.h>

class ConfigurationModalComponent : public UIComponent
{
private:
    bool show_example_text;
    bool show_configuration_modal;
    bool show_demo_window;
public:
    ConfigurationModalComponent();
    void Render() override;
};