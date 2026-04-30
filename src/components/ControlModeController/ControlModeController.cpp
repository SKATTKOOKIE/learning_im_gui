#include "ControlModeController.h"
#include "components/websocket/WebSocketClient.h"
#include "components/websocket/MessageBuilder.h"
#include "components/Protocol/ControlMode.h"
#include <iostream>

ControlModeController::ControlModeController(std::shared_ptr<WebSocketClient> client,
                                           std::shared_ptr<WebSocketClient> telemetryClient)
    : wsClient_(std::move(client)), wsTelemetryClient_(std::move(telemetryClient))
{
}

void ControlModeController::SendCommand(control_mode::Command cmd)
{
    if (!wsClient_)
    {
        return;
    }

    std::string message =
        CommandMessageBuilder::BuildControlModeCommand(static_cast<int>(cmd));

    wsClient_->Send(message);
}

void ControlModeController::SetStandby()
{
    SendCommand(control_mode::Command::SET_STANDBY);
}

void ControlModeController::SetActive()
{
    SendCommand(control_mode::Command::SET_ACTIVE);
}

void ControlModeController::HandleTelemetry(const std::string& message)
{
    std::cout << "ControlMode Telemetry RAW: " << message << "\n";
    
    TelemetryMessageParser::ControlModeState telemetryState;

    if (!TelemetryMessageParser::TryParseControlModeState(message, telemetryState))
    {
        std::cout << "Failed to parse control mode telemetry!\n";
        return;
    }

    switch (telemetryState)
    {
        case control_mode::State::UNKNOWN:
            state_ = control_mode::State::UNKNOWN;
            break;
        case control_mode::State::STANDBY:
            state_ = control_mode::State::STANDBY;
            break;
        case control_mode::State::ACTIVE:
            state_ = control_mode::State::ACTIVE;
            break;
        default:
            return;
    }
}