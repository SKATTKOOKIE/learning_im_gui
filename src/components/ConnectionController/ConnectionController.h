#pragma once

#include <memory>
#include <string>
#include <atomic>
#include "components/websocket/MessageBuilder.h"

class WebSocketClient;

// Map our local enum to the IDL enum values
enum class ConnectionState
{
    Unknown = 0,
    Connected = 1,
    Disconnected = 2,
    Connecting = 3,
    Disconnecting = 4,
    Error = 5  // Local-only state
};

class ConnectionController
{
public:
    explicit ConnectionController(std::shared_ptr<WebSocketClient> client);

    void Connect();
    void Disconnect();
    void Update();

    ConnectionState GetState() const { return state_; }
    const std::string& GetLastError() const { return lastError_; }

private:
    void SendCommand(int commandValue); // 0=query, 1=connect, 2=disconnect
    void HandleResponse(const std::string& message);

    std::shared_ptr<WebSocketClient> wsClient_;
    std::atomic<ConnectionState> state_{ ConnectionState::Disconnected };
    std::string lastError_;
};