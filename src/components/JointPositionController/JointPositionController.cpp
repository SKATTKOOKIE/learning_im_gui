#include "JointPositionController.h"
#include "components/websocket/WebSocketClient.h"
#include "components/websocket/MessageBuilder.h"
#include <iostream>

JointPositionController::JointPositionController(std::shared_ptr<WebSocketClient> client)
    : wsClient_(std::move(client))
{
    // Initialize all joints to 0
    for (int i = 0; i < NUM_JOINTS; i++)
    {
        jointPositions_[i] = 0.0f;
        jointVelocities_[i] = 24.0f;  // Default velocity
        jointTorques_[i] = 50.0f;      // Default torque (50%)
    }
}

void JointPositionController::SetJointPosition(int jointIndex, float position, float velocity, float torque)
{
    if (jointIndex < 0 || jointIndex >= NUM_JOINTS)
    {
        lastError_ = "Invalid joint index: " + std::to_string(jointIndex);
        std::cerr << "JointPositionController: " << lastError_ << "\n";
        return;
    }

    // Store the values
    jointPositions_[jointIndex] = position;
    jointVelocities_[jointIndex] = velocity;
    jointTorques_[jointIndex] = torque;

    // Send command to device
    SendJointCommand(jointIndex, position, velocity, torque);
}

void JointPositionController::SendJointCommand(int jointIndex, float position, float velocity, float torque)
{
    if (!wsClient_)
    {
        lastError_ = "WebSocket client not initialized";
        std::cerr << "JointPositionController: " << lastError_ << "\n";
        return;
    }

    // Convert torque from percentage (0-100) to scale (0-100.00)
    float torqueScale = torque;  // torque is already in the correct scale

    // Build message structure:
    // {"command": {"type": 3, "data": {"joint": <int>, "position": <float>, "velocity": <float>, "torque": <float>}}}
    JsonMessageBuilder dataBuilder;
    dataBuilder.AddField("joint", jointIndex)
               .AddField("position", position)
               .AddField("velocity", velocity)
               .AddField("torque", torqueScale);

    JsonMessageBuilder commandBuilder;
    commandBuilder.AddField("type", 3)  // COMMAND_TYPE_JOINT_POSITION
                  .AddObject("data", dataBuilder);

    JsonMessageBuilder rootBuilder;
    rootBuilder.AddObject("command", commandBuilder);

    std::string message = rootBuilder.Build();
    std::cout << "JointPositionController: Sending joint command - joint: " << jointIndex 
              << ", position: " << position << ", velocity: " << velocity 
              << ", torque: " << torqueScale << "\n";
    std::cout << "JointPositionController: Message: " << message << "\n";

    wsClient_->Send(message);
}

void JointPositionController::Update()
{
    if (!wsClient_)
        return;

    std::string message;
    while (wsClient_->PollMessage(message))
        HandleResponse(message);
}

void JointPositionController::HandleResponse(const std::string& message)
{
    std::cout << "JointPositionController: Received message: " << message << "\n";

    ResponseMessageParser::ResponseCode code;
    std::string description;

    if (!ResponseMessageParser::TryParse(message, code, description))
    {
        // Not a response message, might be telemetry - ignore silently
        std::cout << "JointPositionController: Not a response message, ignoring\n";
        return;
    }

    std::cout << "JointPositionController: Parsed response - code: " << static_cast<int>(code) 
              << ", description: '" << description << "'\n";

    // Handle response based on code
    switch (code)
    {
        case ResponseMessageParser::ResponseCode::Accepted:
        {
            std::cout << "JointPositionController: Command accepted\n";
            break;
        }

        case ResponseMessageParser::ResponseCode::Rejected:
        {
            lastError_ = "Joint command rejected: " + description;
            std::cerr << "JointPositionController: " << lastError_ << "\n";
            break;
        }

        default:
            lastError_ = "Unknown response code: " + std::to_string(static_cast<int>(code));
            std::cerr << "JointPositionController: " << lastError_ << "\n";
            break;
    }
}

float JointPositionController::GetJointPosition(int jointIndex) const
{
    if (jointIndex < 0 || jointIndex >= NUM_JOINTS)
        return 0.0f;
    return jointPositions_[jointIndex];
}

float JointPositionController::GetJointVelocity(int jointIndex) const
{
    if (jointIndex < 0 || jointIndex >= NUM_JOINTS)
        return 0.0f;
    return jointVelocities_[jointIndex];
}

float JointPositionController::GetJointTorque(int jointIndex) const
{
    if (jointIndex < 0 || jointIndex >= NUM_JOINTS)
        return 0.0f;
    return jointTorques_[jointIndex];
}