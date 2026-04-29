#pragma once

#include <memory>
#include "components/ControlModeController/ControlModeController.h"

class ControlModeView
{
public:
    explicit ControlModeView(std::shared_ptr<ControlModeController> controller);

    void Open();
    void Close();
    void Draw();
    void DrawStateIndicator() const;
    void DrawButtons();

private:
    std::shared_ptr<ControlModeController> controller_;
    bool isOpen_ = true;
    
};