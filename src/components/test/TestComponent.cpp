#include "components/test/TestComponent.h"
#include <imgui.h>

TestComponent::TestComponent()
    : show_example_text(false)
{
}

void TestComponent::Render()
{
    ImGui::Begin("This is a test window for ImGui");

    ImGui::Checkbox("Unhide the text", &show_example_text);

    if (show_example_text)
    {
        ImGui::Text("I am the hidden text!");
    }

    ImGui::End();
}