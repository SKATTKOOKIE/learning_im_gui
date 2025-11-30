#include "core/Application.h"
#include "components/test/TestComponent.h"

// This include is just a test
#include "components/test2/TestComponent2.h"

#include <memory>

int main()
{
    Application app(1920, 1080, "test window");
    
    if (!app.Initialize())
    {
        return -1;
    }
    
    // Add the test component
    auto testComponent = std::make_shared<TestComponent>();
    app.AddComponent(testComponent);

	auto testComponent2 = std::make_shared<TestComponent2>();
	app.AddComponent(testComponent2);
    
    app.Run();
    
    return 0;
}