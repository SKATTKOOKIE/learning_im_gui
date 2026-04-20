#include "core/Application.h"
#include "components/layout/LayoutManager.h"
#include "components/test/TestComponent.h"
#include "components/ConfigurationModal/ConfigurationModal.h"
#include "components/websocket/WebSocketClient.h"
#include "components/websocketmonitor/WebSocketMonitor.h"
#include "components/ConnectionController/ConnectionController.h"
#include "components/ConnectionView/ConnectionView.h"
#include "components/ControlModeController/ControlModeController.h"
#include "components/ControlModeView/ControlModeView.h"
#include <memory>
#include <iostream>
#include "imgui_demo.cpp"

int main()
{
    Application app(1920, 1080, "test window");
    if (!app.Initialize())
    {
        return -1;
    }

    // --- WebSocket Clients Setup ---
    
    // Command port (8765)
    auto wsCommandClient = std::make_shared<WebSocketClient>("172.19.170.243", "8765");
    wsCommandClient->Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (wsCommandClient->IsConnected())
    {
        std::cout << "WebSocket (Command) connected!" << std::endl;
    }
    else
    {
        std::cout << "WebSocket (Command) failed to connect" << std::endl;
    }

    // Telemetry port (8766)
    auto wsTelemetryClient = std::make_shared<WebSocketClient>("172.19.170.243", "8766");
    wsTelemetryClient->Start();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (wsTelemetryClient->IsConnected())
    {
        std::cout << "WebSocket (Telemetry) connected!" << std::endl;
    }
    else
    {
        std::cout << "WebSocket (Telemetry) failed to connect" << std::endl;
    }

    // --- Connection Controller and View ---
    auto connectionController = std::make_shared<ConnectionController>(wsCommandClient);
    auto connectionView       = std::make_shared<ConnectionView>(connectionController);

    // --- Connection Controller and View ---
    auto controlModeController = std::make_shared<ControlModeController>(wsCommandClient);
    auto controlModeView       = std::make_shared<ControlModeView>(controlModeController);

    // --- Create WebSocket Monitors ---
    auto wsCommandMonitor   = std::make_shared<WebSocketMonitor>(wsCommandClient);
    auto wsTelemetryMonitor = std::make_shared<WebSocketMonitor>(wsTelemetryClient);

    // --- Layout Manager and Components ---
    auto layoutManager = std::make_shared<LayoutManager>();
    auto testComponent = std::make_shared<TestComponent>();

    auto configurationModal = std::make_shared<ConfigurationModalComponent>();
    layoutManager->AddWidget(DockZone::Right, configurationModal);

    // Add widgets to layout
    layoutManager->AddWidget(DockZone::Left, testComponent);
    layoutManager->AddWidget(DockZone::Bottom, wsCommandMonitor);
    layoutManager->AddWidget(DockZone::Bottom, wsTelemetryMonitor);

    app.AddComponent(layoutManager);

    // --- Set Update Callback ---
    app.SetUpdateCallback([&]() {
        wsCommandMonitor->Update();
        wsTelemetryMonitor->Update();
        connectionController->Update();
        connectionView->Draw();
        controlModeView->Draw();
    });

    // --- Run Application ---
    app.Run();

    // --- Cleanup ---
    std::cout << "Stopping WebSockets..." << std::endl;
    wsCommandClient->Stop();
    wsTelemetryClient->Stop();

    return 0;
}