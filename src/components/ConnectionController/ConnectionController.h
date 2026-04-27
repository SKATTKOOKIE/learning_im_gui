#pragma once

#include "components/websocket/WebSocketClient.h"
#include <atomic>
#include <memory>
#include <string>

enum class ConnectionState
{
    Unknown = 0,
    Connected = 1,
    Disconnected = 2,
    Connecting = 3,
    Disconnecting = 4,
    Error = 5
};

class ConnectionController
{
public:
    explicit ConnectionController(std::shared_ptr<WebSocketClient> client,
                                  std::shared_ptr<WebSocketClient> telemetryClient = nullptr);

    void Connect();
    void Disconnect();
    void Update();

    [[nodiscard]] ConnectionState GetState() const { return state_; }
    [[nodiscard]] const std::string& GetLastError() const { return lastError_; }

    // Set the telemetry client after construction if needed
    void SetTelemetryClient(std::shared_ptr<WebSocketClient> telemetryClient)
    {
        wsTelemetryClient_ = std::move(telemetryClient);
    }

private:
    void SendCommand(int commandValue);
    void HandleResponse(const std::string& message);
    void HandleTelemetry(const std::string& message);

    std::shared_ptr<WebSocketClient> wsClient_;
    std::shared_ptr<WebSocketClient> wsTelemetryClient_;
    std::atomic<ConnectionState> state_ = ConnectionState::Unknown;
    std::string lastError_;
};