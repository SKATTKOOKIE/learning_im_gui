#pragma once
#include <memory>
#include "core/UIComponent.h"
#include "components/ConnectionView/ConnectionView.h"
#include "components/ControlModeView/ControlModeView.h"
#include "components/ConfigurationModal/ConfigurationModal.h"

class LeftColumnWidget : public UIComponent
{
public:
    LeftColumnWidget(std::shared_ptr<ConnectionView> connectionView,
                     std::shared_ptr<ControlModeView> controlModeView,
                     std::shared_ptr<ConfigurationModalComponent> configModal);
    void Render() override;
    void SetWindow(GLFWwindow* win) override;

private:
    std::shared_ptr<ConnectionView>              connectionView_;
    std::shared_ptr<ControlModeView>             controlModeView_;
    std::shared_ptr<ConfigurationModalComponent> configModal_;
};