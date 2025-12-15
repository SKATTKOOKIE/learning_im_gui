#include "components/websocketmonitor/WebSocketMonitor.h"
#include "components/websocket/WebSocketClient.h"
#include <imgui.h>
#include <sstream>
#include <iomanip>
#include <cstring>

WebSocketMonitor::WebSocketMonitor(std::shared_ptr<WebSocketClient> client)
    : wsClient_(client), autoScroll_(true), maxMessages_(100)
{
    memset(inputBuffer_, 0, sizeof(inputBuffer_));
}

void WebSocketMonitor::Render()
{
    ImGui::Begin("WebSocket Monitor");

    // Connection status section
    ImGui::Text("Connection Status:");
    ImGui::SameLine();
    if (wsClient_->IsConnected())
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "CONNECTED");
    }
    else
    {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "DISCONNECTED");
    }

    ImGui::Separator();

    // Message input section
    ImGui::Text("Send Message:");
    ImGui::PushItemWidth(-100);
    bool enterPressed = ImGui::InputText("##input", inputBuffer_, sizeof(inputBuffer_), 
                                         ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::PopItemWidth();
    
    ImGui::SameLine();
    bool sendClicked = ImGui::Button("Send");

    if ((enterPressed || sendClicked) && strlen(inputBuffer_) > 0)
    {
        std::string msg(inputBuffer_);
        wsClient_->Send(msg);
        AddMessage(msg, true);
        memset(inputBuffer_, 0, sizeof(inputBuffer_));
    }

    // Quick test buttons
    ImGui::SameLine();
    if (ImGui::Button("Ping"))
    {
        wsClient_->Send("ping");
        AddMessage("ping", true);
    }

    ImGui::Separator();

    // Options
    ImGui::Checkbox("Auto-scroll", &autoScroll_);
    ImGui::SameLine();
    if (ImGui::Button("Clear"))
    {
        messages_.clear();
    }
    ImGui::SameLine();
    ImGui::Text("Messages: %d", (int)messages_.size());

    ImGui::Separator();

    // Messages display section
    ImGui::BeginChild("MessageScroll", ImVec2(0, 0), true);

    for (const auto& msg : messages_)
    {
        // Color code: green for sent, cyan for received
        if (msg.isSent)
        {
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "[SENT]");
        }
        else
        {
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "[RECV]");
        }
        
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", FormatTimestamp(msg.timestamp).c_str());
        
        ImGui::SameLine();
        ImGui::Text("%s", msg.text.c_str());
    }

    // Auto-scroll to bottom
    if (autoScroll_ && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();

    ImGui::End();
}

void WebSocketMonitor::Update()
{
    // Poll for new messages
    std::string msg;
    while (wsClient_->PollMessage(msg))
    {
        AddMessage(msg, false);
    }
}

void WebSocketMonitor::AddMessage(const std::string& text, bool isSent)
{
    Message msg;
    msg.text = text;
    msg.isSent = isSent;
    msg.timestamp = std::chrono::system_clock::now();
    
    messages_.push_back(msg);
    
    // Limit message history
    if (messages_.size() > maxMessages_)
    {
        messages_.erase(messages_.begin());
    }
}

std::string WebSocketMonitor::FormatTimestamp(const std::chrono::system_clock::time_point& time)
{
    auto now = std::chrono::system_clock::to_time_t(time);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        time.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    std::tm tm;
    localtime_s(&tm, &now);
    ss << std::put_time(&tm, "%H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}