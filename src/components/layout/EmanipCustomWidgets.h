#pragma once
#include <imgui.h>

class EmanipCustomWidgets
{
public:
    static bool PushButton(const char* label,
                       ImVec2 size = ImVec2(120, 50),
                       ImVec4 borderColor = ImVec4(0.f, 0.494f, 0.467f, 1.f));
};