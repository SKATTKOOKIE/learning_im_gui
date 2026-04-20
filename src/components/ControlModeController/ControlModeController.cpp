#include "ControlModeController.h"
#include "components/websocket/WebSocketClient.h"
#include <iostream>

ControlModeController::ControlModeController(std::shared_ptr<WebSocketClient> client)
    : wsClient_(std::move(client))
{
}

void ControlModeController::SendCommand(ControlModeCommands command)
{
    if (!wsClient_)
        return;

    std::string message =
        CommandMessageBuilder::BuildControlModeCommand(static_cast<int>(command));

    wsClient_->Send(message);
}

void ControlModeController::SetStandby()
{
    SendCommand(ControlModeCommands::CONTROL_MODE_COMMAND_SET_STANDBY);
}

void ControlModeController::SetActive()
{
    SendCommand(ControlModeCommands::CONTROL_MODE_COMMAND_SET_ACTIVE);
}
