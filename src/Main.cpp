#include "core/Application.h"
#include "components/layout/LayoutManager.h"
#include "components/ConfigurationModal/ConfigurationModal.h"
#include "components/websocket/WebSocketClient.h"
#include "components/websocketmonitor/WebSocketMonitor.h"
#include "components/ConnectionController/ConnectionController.h"
#include "components/ConnectionView/ConnectionView.h"
#include "components/ControlModeController/ControlModeController.h"
#include "components/ControlModeView/ControlModeView.h"
#include "components/TestVerticalSlider/TestVerticalSlider.h"
#include "components/JointPositionController/JointPositionController.h"
#include "components/websocket/TelemetryDispatcher.h"
#include "components/layout/LeftColumnWidget.h"
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
    auto wsCommandClient = std::make_shared<WebSocketClient>("172.19.171.29", "8765");
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
    auto wsTelemetryClient = std::make_shared<WebSocketClient>("172.19.171.29", "8766");
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
    auto connectionController = std::make_shared<ConnectionController>(wsCommandClient, wsTelemetryClient);
    auto connectionView       = std::make_shared<ConnectionView>(connectionController);

    // --- Control Mode Controller and View ---
    auto controlModeController = std::make_shared<ControlModeController>(wsCommandClient, wsTelemetryClient);
    auto controlModeView       = std::make_shared<ControlModeView>(controlModeController);

    auto configurationModal = std::make_shared<ConfigurationModalComponent>();

    // This will hosue connection view and control mode view widgets
    auto leftColumn = std::make_shared<LeftColumnWidget>(connectionView, controlModeView, configurationModal);

    // --- Joint Position Controller ---
    auto jointPositionController = std::make_shared<JointPositionController>(wsCommandClient);

    // --- Create WebSocket Monitors ---
    auto wsCommandMonitor   = std::make_shared<WebSocketMonitor>(wsCommandClient);

    // --- Create Telemetry Dispatcher ---
    auto telemetryDispatcher = std::make_shared<TelemetryDispatcher>(wsTelemetryClient);
    telemetryDispatcher->RegisterConnectionController(connectionController);
    telemetryDispatcher->RegisterControlModeController(controlModeController);
    telemetryDispatcher->RegisterJointPositionController(jointPositionController);

    // --- Layout Manager and Components ---
    auto layoutManager = std::make_shared<LayoutManager>();

    auto verticalSlider = std::make_shared<TestVerticalSlider>();
    verticalSlider->SetController(jointPositionController);
  
    // Add widgets to layout
    layoutManager->AddWidget(DockZone::Left, leftColumn);
    layoutManager->AddWidget(DockZone::Bottom, wsCommandMonitor);
    layoutManager->AddWidget(DockZone::Center, verticalSlider);

    app.AddComponent(layoutManager);

    // --- Set Update Callback ---
    app.SetUpdateCallback([&]() 
    {
        // Dispatch all telemetry messages first
        telemetryDispatcher->Update();
        
        // Then update views
        wsCommandMonitor->Update();
        jointPositionController->Update();
    });

    // --- Run Application ---
    app.Run();

    // --- Cleanup ---
    std::cout << "Stopping WebSockets..." << std::endl;
    wsCommandClient->Stop();
    wsTelemetryClient->Stop();

    return 0;
}