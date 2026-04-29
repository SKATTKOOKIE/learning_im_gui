#include "EmanipCustomWidgets.h"

bool EmanipCustomWidgets::PushButton(const char* label,
                                     ImVec2 button_size,
                                     ImVec4 borderColor)
{
    ImGui::PushID(label);

    float rounding = 8.0f;
    float border_thickness = 3.0f;

    ImVec2 p = ImGui::GetCursorScreenPos();
    bool is_clicked = ImGui::InvisibleButton(label, button_size);

    bool is_hovered = ImGui::IsItemHovered();
    bool is_active = ImGui::IsItemActive();

    float press_offset = is_active ? 2.0f : 0.0f;
    p.y += press_offset;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImU32 colour_bg = ImGui::GetColorU32(
        is_hovered ? ImVec4(0.25f, 0.25f, 0.25f, 1.f)
                   : ImVec4(0.149f, 0.149f, 0.149f, 1.f));

    ImU32 colour_border = ImGui::GetColorU32(borderColor);
    ImU32 colour_text = ImGui::GetColorU32(ImVec4(1, 1, 1, 1));

    if (is_hovered)
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    // Button
    draw_list->AddRectFilled(
        p,
        ImVec2(p.x + button_size.x, p.y + button_size.y),
        colour_bg,
        rounding
    );

    // Bottom border
    if (!is_active)
    {
        draw_list->AddRectFilled(
            ImVec2(p.x, p.y + button_size.y - border_thickness),
            ImVec2(p.x + button_size.x, p.y + button_size.y),
            colour_border,
            rounding,
            ImDrawFlags_RoundCornersBottom
        );
    }

    // Text
    ImVec2 text_size = ImGui::CalcTextSize(label);
    ImVec2 text_pos = ImVec2(
        p.x + (button_size.x - text_size.x) * 0.5f,
        p.y + (button_size.y - text_size.y) * 0.5f
    );

    draw_list->AddText(text_pos, colour_text, label);

    ImGui::PopID();
    return is_clicked;
}