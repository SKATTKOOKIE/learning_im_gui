#pragma once

#include <memory>
#include <string>
#include <atomic>
#include "components/websocket/MessageBuilder.h"

class WebSocketClient;

enum class ControlModeCommands
{
    CONTROL_MODE_COMMAND_NONE = 0,
    CONTROL_MODE_COMMAND_SET_STANDBY = 1,
    CONTROL_MODE_COMMAND_SET_ACTIVE = 2,
};

enum class ControlModeStates
{
    CONTROL_MODE_STATE_UNKNOWN = 0,
    CONTROL_MODE_STATE_STANDBY = 1,
    CONTROL_MODE_STATE_ACTIVE = 2,
    CONTROL_MODE_STATE_SAFE = 3,
    CONTROL_MODE_STATE_E_STOP = 4,
};

class ControlModeController
{
public:
    explicit ControlModeController(std::shared_ptr<WebSocketClient> client);

    void SetStandby();
    void SetActive();

    const std::string& GetLastError() const { return lastError_; }

private:
    void SendCommand(ControlModeCommands command);
    void HandleResponse(const std::string& message);

    std::shared_ptr<WebSocketClient> wsClient_;
    std::atomic<ControlModeStates> state_{ ControlModeStates::CONTROL_MODE_STATE_UNKNOWN };
    std::string lastError_;
};