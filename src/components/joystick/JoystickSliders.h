// Generated with ImRAD 0.8
// visit https://github.com/tpecholt/imrad

#pragma once
#include <imgui.h>
#include <memory>
#include <thread>
#include <atomic>
#include <array>
#include <string>
#include "components/websocket/MessageBuilder.h"
#include "components/structures/JoystickMessageStructure.h"

class WebSocketClient;

class JoystickSliders
{
public:
    JoystickSliders();
    ~JoystickSliders();

    /// @begin interface
    void Open();
    void Close();
    void Draw();

    void SetWebSocketClient(std::shared_ptr<WebSocketClient> client);

    // Single slider values instead of Int3 arrays
    int xDemand = 0;
    int yDemand = 0;
    int zDemand = 0;
    int rollDemand = 0;
    int pitchDemand = 0;
    int yawDemand = 0;
    /// @end interface

private:
    /// @begin impl
    bool isOpen = true;
    bool isSending = false;
    
    std::shared_ptr<WebSocketClient> wsClient_;
    std::thread sendThread_;
    std::atomic<bool> shouldSendLoop_{false};
    
    // Message structure components
    MessageEnvelope envelope_;
    mutable JoystickDataPayload dataPayload_;  // mutable because BuildMessage() is const
    
    void StartSending();
    void StopSending();
    void SendLoop();
    std::string BuildMessage() const;
    /// @end impl
};

extern JoystickSliders joystickSliders;