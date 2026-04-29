#pragma once

#include <memory>
#include <atomic>
#include "components/websocket/MessageBuilder.h"

class WebSocketClient;

class JointPositionController
{
public:
    static constexpr int NUM_JOINTS = 8;

    explicit JointPositionController(std::shared_ptr<WebSocketClient> client);

    void SetJointPosition(int jointIndex, float position, float velocity, float torque);
    void HandleTelemetry(const std::string& message);  // ← NEW
    void Update();

    float GetJointPosition(int jointIndex) const;
    float GetJointVelocity(int jointIndex) const;
    float GetJointTorque(int jointIndex) const;
    
    const std::string& GetLastError() const { return lastError_; }

    void SendJointCommand(int jointIndex, float position, float velocity, float torque);
    void SendJawCommand(int command, float velocity, float torque);

private:
    void HandleResponse(const std::string& message);

    std::shared_ptr<WebSocketClient> wsClient_;
    
    // Store current joint states
    float jointPositions_[NUM_JOINTS]{};
    float jointVelocities_[NUM_JOINTS]{};
    float jointTorques_[NUM_JOINTS]{};
    
    std::string lastError_;
};