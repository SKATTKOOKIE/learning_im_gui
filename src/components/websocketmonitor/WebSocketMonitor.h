#pragma once
#include "core/UIComponent.h"
#include <memory>
#include <vector>
#include <string>
#include <chrono>

class WebSocketClient;

struct Message
{
    std::string text;
    bool isSent; // true = sent, false = received
    std::chrono::system_clock::time_point timestamp;
};

class WebSocketMonitor : public UIComponent
{
public:
    WebSocketMonitor(std::shared_ptr<WebSocketClient> client);
    void Render() override;
    void Update(); // Call this in your main loop

private:
    std::shared_ptr<WebSocketClient> wsClient_;
    std::vector<Message> messages_;
    char inputBuffer_[256];
    bool autoScroll_;
    int maxMessages_;
    
    void AddMessage(const std::string& text, bool isSent);
    std::string FormatTimestamp(const std::chrono::system_clock::time_point& time);
};