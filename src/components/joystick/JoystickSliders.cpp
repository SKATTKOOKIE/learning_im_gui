// Generated with ImRAD 0.8
// visit https://github.com/tpecholt/imrad

#include "components/joystick/JoystickSliders.h"
#include "components/websocket/WebSocketClient.h"
#include "components/structures/JoystickMessageStructure.h"
#include <chrono>
#include <thread>

JoystickSliders joystickSliders;

JoystickSliders::JoystickSliders()
{
    // Configure the message envelope
    envelope_.messageType = "rov";
    envelope_.messageId = 1050;
    envelope_.deviceId = 0;
    envelope_.version = "0.9.0";
    envelope_.sessionId = 1000;
}

JoystickSliders::~JoystickSliders()
{
    StopSending();
}

void JoystickSliders::Open()
{
    isOpen = true;
}

void JoystickSliders::Close()
{
    isOpen = false;
}

void JoystickSliders::SetWebSocketClient(std::shared_ptr<WebSocketClient> client)
{
    wsClient_ = client;
}

void JoystickSliders::Draw()
{
    /// @style Dark
    /// @unit px
    /// @begin TopWindow
    ImGui::SetNextWindowSize({ 640, 480 }, ImGuiCond_FirstUseEver);
    if (isOpen && ImGui::Begin("joystick_controller###JoystickSliders", &isOpen, ImGuiWindowFlags_NoCollapse))
    {
        // Connection status
        if (wsClient_)
        {
            if (wsClient_->IsConnected())
            {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "WebSocket: CONNECTED");
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "WebSocket: DISCONNECTED");
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "WebSocket: NOT CONFIGURED");
        }
        
        ImGui::SameLine();
        if (isSending)
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "● SENDING");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "○ STOPPED");
        }

        ImGui::Separator();

        /// @begin Sliders
        ImGui::Text("X Demand: %d", xDemand);
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderInt("##X", &xDemand, 0, 10000);

        ImGui::Text("Y Demand: %d", yDemand);
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderInt("##Y", &yDemand, 0, 10000);

        ImGui::Text("Z Demand: %d", zDemand);
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderInt("##Z", &zDemand, 0, 10000);

        ImGui::Text("Roll Demand: %d", rollDemand);
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderInt("##Roll", &rollDemand, 0, 10000);

        ImGui::Text("Pitch Demand: %d", pitchDemand);
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderInt("##Pitch", &pitchDemand, 0, 10000);

        ImGui::Text("Yaw Demand: %d", yawDemand);
        ImGui::SetNextItemWidth(-1);
        ImGui::SliderInt("##Yaw", &yawDemand, 0, 10000);
        /// @end Sliders

        ImGui::Separator();

        /// @begin Button
        bool canSend = wsClient_ && wsClient_->IsConnected();
        
        if (!isSending)
        {
            if (!canSend)
                ImGui::BeginDisabled();
                
            if (ImGui::Button("Start Sending (10Hz)", { 150, 30 }))
            {
                StartSending();
            }
            
            if (!canSend)
            {
                ImGui::EndDisabled();
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                    ImGui::SetTooltip("WebSocket is not connected");
            }
            else if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
                ImGui::SetTooltip("Start sending joystick data at 10Hz to the topside API");
            }
        }
        else
        {
            if (ImGui::Button("Stop Sending", { 150, 30 }))
            {
                StopSending();
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                ImGui::SetTooltip("Stop sending joystick data to the topside API");
        }
        /// @end Button

        /// @separator
        ImGui::End();
    }
    /// @end TopWindow
}

void JoystickSliders::StartSending()
{
    if (isSending || !wsClient_ || !wsClient_->IsConnected())
        return;

    isSending = true;
    shouldSendLoop_ = true;
    sendThread_ = std::thread(&JoystickSliders::SendLoop, this);
}

void JoystickSliders::StopSending()
{
    if (!isSending)
        return;

    shouldSendLoop_ = false;
    if (sendThread_.joinable())
        sendThread_.join();
    
    isSending = false;
}

void JoystickSliders::SendLoop()
{
    using namespace std::chrono;
    
    const auto targetInterval = milliseconds(100); // 10Hz = 100ms
    auto nextSendTime = steady_clock::now();

    while (shouldSendLoop_)
    {
        // Build and send the JSON message
        std::string jsonMsg = BuildMessage();
        
        if (wsClient_ && wsClient_->IsConnected())
        {
            wsClient_->Send(jsonMsg);
        }
        else
        {
            // Lost connection, stop sending
            shouldSendLoop_ = false;
            isSending = false;
            break;
        }

        // Calculate next send time
        nextSendTime += targetInterval;
        
        // Sleep until next send time
        std::this_thread::sleep_until(nextSendTime);
        
        // If we've fallen behind, reset to current time
        auto now = steady_clock::now();
        if (nextSendTime < now)
        {
            nextSendTime = now;
        }
    }
}

std::string JoystickSliders::BuildMessage() const
{
    // Update the data payload with current slider values
    dataPayload_.x = xDemand;
    dataPayload_.y = yDemand;
    dataPayload_.z = zDemand;
    dataPayload_.r = rollDemand;
    dataPayload_.p = pitchDemand;
    dataPayload_.h = yawDemand;
    
    // Build the complete message with envelope + payload
    return envelope_.BuildWithPayload(dataPayload_);
}