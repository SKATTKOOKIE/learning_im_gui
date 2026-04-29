#include "TelemetryDispatcher.h"
#include "components/websocket/WebSocketClient.h"
#include "components/ConnectionController/ConnectionController.h"
#include "components/ControlModeController/ControlModeController.h"
#include "components/JointPositionController/JointPositionController.h"
#include "components/JointStatusTable/JointStatusTable.h"
#include <iostream>
#include <cstdio>

TelemetryDispatcher::TelemetryDispatcher(std::shared_ptr<WebSocketClient> telemetryClient)
    : wsTelemetryClient_(std::move(telemetryClient))
{
}

void TelemetryDispatcher::RegisterConnectionController(std::shared_ptr<ConnectionController> controller)
{
    connectionController_ = std::move(controller);
}

void TelemetryDispatcher::RegisterControlModeController(std::shared_ptr<ControlModeController> controller)
{
    controlModeController_ = std::move(controller);
}

void TelemetryDispatcher::RegisterJointPositionController(std::shared_ptr<JointPositionController> controller)
{
    jointPositionController_ = std::move(controller);
}

void TelemetryDispatcher::RegisterJointStatusTable(std::shared_ptr<JointStatusTable> table)
{
    jointStatusTable_ = std::move(table);
}

int TelemetryDispatcher::ExtractTelemetryType(const std::string& message) const
{
    // Extract type field from telemetry message
    // Expected format: {"telemetry": {"type": <int>, ...}}
    
    const std::string typeSearch = "\"type\":";
    size_t typePos = message.find(typeSearch);
    if (typePos == std::string::npos)
        return -1;
    
    typePos += typeSearch.size();
    int typeValue = 0;
    
    if (sscanf(message.c_str() + typePos, "%d", &typeValue) != 1)
        return -1;
    
    return typeValue;
}

void TelemetryDispatcher::Update()
{
    if (!wsTelemetryClient_)
    {
        std::cout << "TelemetryDispatcher: No telemetry client!\n";
        return;
    }

    // Poll all available messages
    std::string message;
    while (wsTelemetryClient_->PollMessage(message))
    {
        // std::cout << "TelemetryDispatcher received: " << message << "\n";
        
        int telemetryType = ExtractTelemetryType(message);
        
        // Route to appropriate controller based on type
        switch (telemetryType)
        {
            case 1:  // Connection telemetry
                if (connectionController_)
                {
                    // std::cout << "TelemetryDispatcher: Routing to ConnectionController\n";
                    connectionController_->HandleTelemetry(message);
                }
                break;
                
            case 2:  // Control Mode telemetry
                if (controlModeController_)
                {
                    // std::cout << "TelemetryDispatcher: Routing to ControlModeController\n";
                    controlModeController_->HandleTelemetry(message);
                }
                break;
                
            case 3:  // Joint Position telemetry
                if (jointPositionController_)
                {
                    std::cout << "TelemetryDispatcher: Routing to JointPositionController\n";
                    jointPositionController_->HandleTelemetry(message);
                }
                break;
               
            case 4:  // Joint STATUS telemetry
                if (jointStatusTable_)
                {
                    std::cout << "TelemetryDispatcher: Routing to jointStatusTable\n";
                    jointStatusTable_->HandleTelemetry(message);
                }
                break;   

            default:
                std::cout << "TelemetryDispatcher: Unknown telemetry type: " << telemetryType << "\n";
        }
    }
}