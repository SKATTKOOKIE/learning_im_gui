#include "ConnectionController.h"
#include "components/websocket/WebSocketClient.h"
#include "components/websocket/MessageBuilder.h"
#include <iostream>

ConnectionController::ConnectionController(std::shared_ptr<WebSocketClient> client,
                                           std::shared_ptr<WebSocketClient> telemetryClient)
    : wsClient_(std::move(client)), wsTelemetryClient_(std::move(telemetryClient))
{
}

void ConnectionController::Connect()
{
    if (!wsClient_)
        return;

    SendCommand(1); // fire and forget
}

void ConnectionController::Disconnect()
{
    if (!wsClient_)
        return;

    SendCommand(2); // fire and forget
}

void ConnectionController::SendCommand(int commandValue)
{
    // Use existing CommandMessageBuilder to build connect command
    std::string message = CommandMessageBuilder::BuildConnectCommand(commandValue);
    wsClient_->Send(message);
}

void ConnectionController::HandleTelemetry(const std::string& message)
{
    TelemetryMessageParser::ConnectionState telemetryState;

    if (!TelemetryMessageParser::TryParseConnectionState(message, telemetryState))
    {
        return;
    }

    switch (telemetryState)
    {
        case TelemetryMessageParser::ConnectionState::Unknown:
            state_ = ConnectionState::Unknown;
            break;

        case TelemetryMessageParser::ConnectionState::Connected:
            state_ = ConnectionState::Connected;
            break;

        case TelemetryMessageParser::ConnectionState::Disconnected:
            state_ = ConnectionState::Disconnected;
            break;

        default:
            return; // ignore anything else
    }
}