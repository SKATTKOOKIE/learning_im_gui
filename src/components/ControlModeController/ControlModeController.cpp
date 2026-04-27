#include "ControlModeController.h"
#include "components/websocket/WebSocketClient.h"
#include "components/websocket/MessageBuilder.h"
#include <iostream>

ControlModeController::ControlModeController(std::shared_ptr<WebSocketClient> client,
                                           std::shared_ptr<WebSocketClient> telemetryClient)
    : wsClient_(std::move(client)), wsTelemetryClient_(std::move(telemetryClient))
{
}

void ControlModeController::SendCommand(ControlModeCommands command)
{
    if (!wsClient_)
    {
        return;
    }

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
        case TelemetryMessageParser::ControlModeState::Unknown:
            state_ = ControlModeStates::CONTROL_MODE_STATE_UNKNOWN;
            break;

        case TelemetryMessageParser::ControlModeState::Standby:
            state_ = ControlModeStates::CONTROL_MODE_STATE_STANDBY;
            break;

        case TelemetryMessageParser::ControlModeState::Active:
            state_ = ControlModeStates::CONTROL_MODE_STATE_ACTIVE;
            break;

        default:
            return; // ignore anything else
    }
}