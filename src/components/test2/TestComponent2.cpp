#include "components/test2/TestComponent2.h"
#include <imgui.h>

TestComponent2::TestComponent2()
    : show_example_text(false)
{
}

void TestComponent2::Render()
{
    ImGui::Begin("This is a copy to test the cmake files");

    // bool show_demo_window = true;

    // ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::End();
}