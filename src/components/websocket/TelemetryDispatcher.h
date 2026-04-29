#pragma once

#include <memory>
#include <string>
#include <vector>


// Forward declarations
class ConnectionController;
class ControlModeController;
class JointPositionController;
class JointStatusTable;
class WebSocketClient;

class TelemetryDispatcher
{
public:
    TelemetryDispatcher(std::shared_ptr<WebSocketClient> telemetryClient);
    
    // Register controllers that will receive telemetry messages
    void RegisterConnectionController(std::shared_ptr<ConnectionController> controller);
    void RegisterControlModeController(std::shared_ptr<ControlModeController> controller);
    void RegisterJointPositionController(std::shared_ptr<JointPositionController> controller);
    void RegisterJointStatusTable(std::shared_ptr<JointStatusTable> table);
    
    // Poll all messages from telemetry client and dispatch them
    void Update();

private:
    std::shared_ptr<WebSocketClient> wsTelemetryClient_;
    std::shared_ptr<ConnectionController> connectionController_;
    std::shared_ptr<ControlModeController> controlModeController_;
    std::shared_ptr<JointPositionController> jointPositionController_;
    std::shared_ptr<JointStatusTable> jointStatusTable_;
    
    // Helper to determine message type
    int ExtractTelemetryType(const std::string& message) const;
};