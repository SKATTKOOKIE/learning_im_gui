#pragma once

#include <memory>
#include "components/ConnectionController/ConnectionController.h"

class ConnectionView
{
public:
    explicit ConnectionView(std::shared_ptr<ConnectionController> controller);

    void Open();
    void Close();
    void Draw();

private:
    void DrawStateIndicator() const;
    void DrawButtons();

    std::shared_ptr<ConnectionController> controller_;
    bool isOpen_ = true;
};