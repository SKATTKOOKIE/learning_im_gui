#include "core/Application.h"
#include "components/layout/LayoutManager.h"
#include "components/test/TestComponent.h"
#include "components/test2/TestComponent2.h"

#include <memory>

int main()
{
    Application app(1920, 1080, "test window");

    if (!app.Initialize())
    {
        return -1;
    }

    // Create the layout manager
    auto layoutManager = std::make_shared<LayoutManager>();
    
    // Create widgets
    auto testComponent = std::make_shared<TestComponent>();
    auto testComponent2 = std::make_shared<TestComponent2>();
    
    // Add widgets to specific zones
    layoutManager->AddWidget(DockZone::Left, testComponent);
    layoutManager->AddWidget(DockZone::Right, testComponent2);
    
    // Add the layout manager as the main component
    app.AddComponent(layoutManager);

    app.Run();

    return 0;
}