#pragma once

#include <memory>
#include <string>
#include "components/Protocol/ControlMode.h"

class WebSocketClient;

enum class ControlModeCommands
{
    CONTROL_MODE_COMMAND_QUERY = 0,
    CONTROL_MODE_COMMAND_SET_STANDBY = 1,
    CONTROL_MODE_COMMAND_SET_ACTIVE = 2,
};

using ControlModeStates = control_mode::State;

class ControlModeController
{
public:
    ControlModeController(std::shared_ptr<WebSocketClient> client,
                         std::shared_ptr<WebSocketClient> telemetryClient);
    
    void SetStandby();
    void SetActive();
    void HandleTelemetry(const std::string& message);
    
    control_mode::State GetState() const { return state_; }

private:
    std::shared_ptr<WebSocketClient> wsClient_;
    std::shared_ptr<WebSocketClient> wsTelemetryClient_;
    control_mode::State state_ = control_mode::State::UNKNOWN;
    
    void SendCommand(control_mode::Command cmd);
};