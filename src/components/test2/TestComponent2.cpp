#include "components/test2/TestComponent2.h"
#include <imgui.h>

TestComponent2::TestComponent2()
    : show_example_text(false)
{
}

void TestComponent2::Render()
{
    ImGui::Begin("This is a copy to test the cmake files");

    ImGui::Checkbox("Unhide the text, perchance", &show_example_text);

    if (show_example_text)
    {
        ImGui::Text("I am the hidden text, but number 2!");
    }

    ImGui::End();
}