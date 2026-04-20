#include "ConnectionController.h"
#include "components/websocket/WebSocketClient.h"
#include <iostream>

ConnectionController::ConnectionController(std::shared_ptr<WebSocketClient> client)
    : wsClient_(std::move(client))
{
}

void ConnectionController::Connect()
{
    if (!wsClient_ || state_ == ConnectionState::Connected
        || state_ == ConnectionState::Connecting)
        return;

    state_ = ConnectionState::Connecting;
    SendCommand(1); // 1 = connect
}

void ConnectionController::Disconnect()
{
    if (!wsClient_ || state_ == ConnectionState::Disconnected
        || state_ == ConnectionState::Disconnecting)
        return;

    state_ = ConnectionState::Disconnecting;
    SendCommand(2); // 2 = disconnect
}

void ConnectionController::SendCommand(int commandValue)
{
    // Build message using new structure:
    // {"command": {"type": 1, "data": {"command": <int>}}}
    std::string message = CommandMessageBuilder::BuildConnectCommand(commandValue);
    wsClient_->Send(message);
}

void ConnectionController::Update()
{
    if (!wsClient_)
        return;

    std::string message;
    while (wsClient_->PollMessage(message))
        HandleResponse(message);
}

void ConnectionController::HandleResponse(const std::string& message)
{
    // Parse response envelope:
    // {"response": {"type": 1, "data": {"code": <int>, "description": "<str>"}}}
    
    std::cout << "ConnectionController: Received message: " << message << "\n";
    
    ResponseMessageParser::ResponseCode code;
    std::string description;
    
    if (!ResponseMessageParser::TryParse(message, code, description))
    {
        // Not a response message, might be telemetry - ignore silently
        std::cout << "ConnectionController: Not a response message, ignoring\n";
        return;
    }

    std::cout << "ConnectionController: Parsed response - code: " << static_cast<int>(code) 
              << ", description: '" << description << "'\n";
    std::cout << "ConnectionController: Current state: " << static_cast<int>(state_.load()) << "\n";

    // Handle response based on code
    switch (code)
    {
        case ResponseMessageParser::ResponseCode::Accepted:
        {
            // Determine state based on description
            if (description.find("Connection successful") != std::string::npos ||
                description.find("connection successful") != std::string::npos)
            {
                std::cout << "ConnectionController: Connection successful - moving to CONNECTED\n";
                state_ = ConnectionState::Connected;
            }
            else if (description.find("Disconnection successful") != std::string::npos ||
                     description.find("disconnection successful") != std::string::npos)
            {
                std::cout << "ConnectionController: Disconnection successful - moving to DISCONNECTED\n";
                state_ = ConnectionState::Disconnected;
            }
            else if (description.find("Already connected") != std::string::npos)
            {
                std::cout << "ConnectionController: Already connected\n";
                state_ = ConnectionState::Connected;
            }
            else if (description.find("Already disconnected") != std::string::npos)
            {
                std::cout << "ConnectionController: Already disconnected\n";
                state_ = ConnectionState::Disconnected;
            }
            else if (description.find("in progress") != std::string::npos ||
                     description.find("In progress") != std::string::npos)
            {
                std::cout << "ConnectionController: Operation in progress - keeping current state\n";
                // Keep current state (Connecting or Disconnecting)
            }
            else if (description.find("Query") != std::string::npos)
            {
                std::cout << "ConnectionController: Query response\n";
                // Query response - don't change state
            }
            else
            {
                std::cout << "ConnectionController: Accepted but unknown description: " << description << "\n";
            }
            break;
        }
        
        case ResponseMessageParser::ResponseCode::Rejected:
        {
            lastError_ = "Connection rejected: " + description;
            state_ = ConnectionState::Error;
            std::cerr << "ConnectionController: " << lastError_ << "\n";
            break;
        }
        
        default:
            lastError_ = "Unknown response code: " + std::to_string(static_cast<int>(code));
            state_ = ConnectionState::Error;
            std::cerr << "ConnectionController: " << lastError_ << "\n";
            break;
    }
    
    std::cout << "ConnectionController: New state: " << static_cast<int>(state_.load()) << "\n\n";
}