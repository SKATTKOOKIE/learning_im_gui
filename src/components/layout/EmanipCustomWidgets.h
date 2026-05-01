#pragma once
#include <imgui.h>
#include <cstdio>

class EmanipCustomWidgets
{
public:
    static bool PushButton(const char* label,
                       ImVec2 size = ImVec2(120, 50),
                       ImVec4 borderColor = ImVec4(0.f, 0.494f, 0.467f, 1.f));

    static bool JointPositionVerticalSlider(const char* id, ImVec2 size, float* value, float min, float max);

    static bool BeginPanel(const char* id, ImVec2 size = {0, 0});
    static void EndPanel(); 

    static void LabelledInputFloat(const char* id, float* value, float width, const char* unit, const char* format = "%.1f")
    {
        char buf[64];
        snprintf(buf, sizeof(buf), format, *value);
        float textWidth  = ImGui::CalcTextSize(buf).x;
        float innerWidth = width - ImGui::GetStyle().FramePadding.x * 2.0f;
        float offset     = (innerWidth - textWidth) * 0.5f;

        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.157f, 0.169f, 0.192f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
            ImVec2(ImGui::GetStyle().FramePadding.x + (offset > 0.0f ? offset : 0.0f),
                ImGui::GetStyle().FramePadding.y));

        ImGui::SetNextItemWidth(width);
        ImGui::InputFloat(id, value, 0.0f, 0.0f, format);

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::Text("%s", unit);
    }
};