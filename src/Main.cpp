#include "core/Application.h"
#include "components/layout/LayoutManager.h"
#include "components/test/TestComponent.h"
#include "components/test2/TestComponent2.h"
#include "components/websocket/WebSocketClient.h"
#include "components/websocketmonitor/WebSocketMonitor.h"
#include "components/joystick/JoystickSliders.h"

#include <memory>
#include <iostream>

int main()
{
    Application app(1920, 1080, "test window");

    if (!app.Initialize())
    {
        return -1;
    }

    // --- WebSocket Client Setup ---
    auto wsClient = std::make_shared<WebSocketClient>("localhost", "8080");
    wsClient->Start();
    
    // Give it a moment to connect
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    if (wsClient->IsConnected())
    {
        std::cout << "WebSocket connected!" << std::endl;
    }
    else
    {
        std::cout << "WebSocket failed to connect" << std::endl;
    }

    // --- Create Components ---
    auto wsMonitor = std::make_shared<WebSocketMonitor>(wsClient);
    
    // Set up joystick sliders with WebSocket
    joystickSliders.SetWebSocketClient(wsClient);

    // --- Layout Manager and Components ---
    auto layoutManager = std::make_shared<LayoutManager>();
    auto testComponent = std::make_shared<TestComponent>();
    auto testComponent2 = std::make_shared<TestComponent2>();

    // Add widgets to layout
    layoutManager->AddWidget(DockZone::Left, testComponent);
    layoutManager->AddWidget(DockZone::Right, testComponent2);
    layoutManager->AddWidget(DockZone::Bottom, wsMonitor);
    
    app.AddComponent(layoutManager);

    // --- Set Update Callback ---
    app.SetUpdateCallback([&wsMonitor]() {
        // Update WebSocket monitor to poll for new messages
        wsMonitor->Update();
        
        // Draw joystick sliders
        joystickSliders.Draw();
    });

    // --- Run Application ---
    app.Run();
    
    // --- Cleanup ---
    std::cout << "Stopping WebSocket..." << std::endl;
    wsClient->Stop();

    return 0;
}