#include "JointPositionController.h"
#include "components/websocket/WebSocketClient.h"
#include "components/websocket/MessageBuilder.h"
#include <iostream>
#include <cstdio>

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

    if (!wsClient_ || !wsClient_->IsConnected())
        return;

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

void JointPositionController::SendJawCommand(int command, float velocity, float torque)
{
    if (!wsClient_ || !wsClient_->IsConnected())
        return;

    JsonMessageBuilder dataBuilder;
    dataBuilder.AddField("command", command)
               .AddField("velocity", (double)velocity)
               .AddField("torque",   (double)torque);

    JsonMessageBuilder commandBuilder;
    commandBuilder.AddField("type", 4)
                  .AddObject("data", dataBuilder);

    JsonMessageBuilder rootBuilder;
    rootBuilder.AddObject("command", commandBuilder);

    std::string message = rootBuilder.Build();
    std::cout << "JointPositionController: Sending jaw command - command: " << command
              << ", velocity: " << velocity << ", torque: " << torque << "\n";
    wsClient_->Send(message);
}

void JointPositionController::HandleTelemetry(const std::string& message)
{
    // Parse joint position telemetry
    // Expected format: {"telemetry": {"type": 3, "data": {"joint": <int>, "velocity": <float>, "position": <float>, "torque": <float>}}}
    
    std::cout << "JointPositionController: Received telemetry: " << message << "\n";
    
    // Extract joint index
    const std::string jointSearch = "\"joint\":";
    size_t jointPos = message.find(jointSearch);
    if (jointPos == std::string::npos)
    {
        std::cout << "JointPositionController: No 'joint' field found\n";
        return;
    }
    
    jointPos += jointSearch.size();
    int jointIndex = 0;
    if (sscanf(message.c_str() + jointPos, "%d", &jointIndex) != 1)
    {
        std::cout << "JointPositionController: Failed to parse joint index\n";
        return;
    }
    
    if (jointIndex < 0 || jointIndex >= NUM_JOINTS)
    {
        std::cout << "JointPositionController: Invalid joint index: " << jointIndex << "\n";
        return;
    }
    
    // Extract position
    const std::string posSearch = "\"position\":";
    size_t posPos = message.find(posSearch);
    if (posPos == std::string::npos)
    {
        std::cout << "JointPositionController: No 'position' field found\n";
        return;
    }
    
    posPos += posSearch.size();
    float position = 0.0f;
    if (sscanf(message.c_str() + posPos, "%f", &position) != 1)
    {
        std::cout << "JointPositionController: Failed to parse position\n";
        return;
    }
    
    // Extract velocity
    const std::string velSearch = "\"velocity\":";
    size_t velPos = message.find(velSearch);
    float velocity = 0.0f;
    if (velPos != std::string::npos)
    {
        velPos += velSearch.size();
        sscanf(message.c_str() + velPos, "%f", &velocity);
    }
    
    // Extract torque
    const std::string torqueSearch = "\"torque\":";
    size_t torquePos = message.find(torqueSearch);
    float torque = 0.0f;
    if (torquePos != std::string::npos)
    {
        torquePos += torqueSearch.size();
        sscanf(message.c_str() + torquePos, "%f", &torque);
    }
    
    // Update internal state
    jointPositions_[jointIndex] = position;
    jointVelocities_[jointIndex] = velocity;
    jointTorques_[jointIndex] = torque;
    
    std::cout << "JointPositionController: Updated joint " << jointIndex 
              << " - position: " << position 
              << ", velocity: " << velocity 
              << ", torque: " << torque << "\n";
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