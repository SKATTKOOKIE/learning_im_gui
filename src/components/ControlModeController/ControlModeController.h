#pragma once

#include <memory>
#include <string>

class WebSocketClient;

enum class ControlModeCommands
{
    CONTROL_MODE_COMMAND_QUERY = 0,
    CONTROL_MODE_COMMAND_SET_STANDBY = 1,
    CONTROL_MODE_COMMAND_SET_ACTIVE = 2,
};

enum class ControlModeStates
{
    CONTROL_MODE_STATE_UNKNOWN = 0,
    CONTROL_MODE_STATE_STANDBY = 1,
    CONTROL_MODE_STATE_ACTIVE = 2,
};

class ControlModeController
{
public:
    ControlModeController(std::shared_ptr<WebSocketClient> client,
                         std::shared_ptr<WebSocketClient> telemetryClient);
    
    void SetStandby();
    void SetActive();
    void HandleTelemetry(const std::string& message);
    
    ControlModeStates GetState() const { return state_; }

private:
    std::shared_ptr<WebSocketClient> wsClient_;
    std::shared_ptr<WebSocketClient> wsTelemetryClient_;
    ControlModeStates state_ = ControlModeStates::CONTROL_MODE_STATE_UNKNOWN;
    
    void SendCommand(ControlModeCommands command);
};