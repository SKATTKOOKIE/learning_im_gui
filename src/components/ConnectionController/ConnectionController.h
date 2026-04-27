#pragma once

#include <memory>
#include <string>

class WebSocketClient;

enum class ConnectionState
{
    Unknown = 0,
    Connected = 1,
    Disconnected = 2,
};

class ConnectionController
{
public:
    ConnectionController(std::shared_ptr<WebSocketClient> client,
                        std::shared_ptr<WebSocketClient> telemetryClient);
    
    void Connect();
    void Disconnect();
    void HandleTelemetry(const std::string& message);
    
    ConnectionState GetState() const { return state_; }

private:
    std::shared_ptr<WebSocketClient> wsClient_;
    std::shared_ptr<WebSocketClient> wsTelemetryClient_;
    ConnectionState state_ = ConnectionState::Unknown;
    
    void SendCommand(int commandValue);
};