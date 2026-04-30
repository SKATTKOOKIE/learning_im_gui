#include "EmanipCustomWidgets.h"
#include <algorithm>

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

bool EmanipCustomWidgets::BeginPanel(const char* id, ImVec2 size)
{
    ImGui::PushStyleColor(ImGuiCol_ChildBg,  ImVec4(0.094f, 0.102f, 0.118f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border,   ImVec4(0.157f, 0.169f, 0.192f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,   8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    return ImGui::BeginChild(id, size, true);
}

void EmanipCustomWidgets::EndPanel()
{
    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

bool EmanipCustomWidgets::JointPositionVerticalSlider(const char* id, ImVec2 size, float* value, float min, float max)
{
    ImDrawList* dl    = ImGui::GetWindowDrawList();

    ImVec2 pos = ImGui::GetCursorScreenPos();

    // Invisible interaction target
    ImGui::InvisibleButton(id, size);
    bool hovered  = ImGui::IsItemHovered();
    bool active   = ImGui::IsItemActive();
    bool changed  = false;

    if (active && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))
    {
        float drag_delta = ImGui::GetIO().MouseDelta.y;
        float range      = max - min;
        float delta      = -(drag_delta / size.y) * range;
        *value           = std::clamp(*value + delta, min, max);
        changed          = true;
    }

    // Track background
    float track_x     = pos.x + size.x * 0.5f - 4.0f;
    float track_w     = 8.0f;
    ImU32 track_bg    = IM_COL32(30, 30, 35, 255);
    ImU32 track_inner = IM_COL32(15, 15, 20, 255);
    dl->AddRectFilled(ImVec2(track_x, pos.y),
                      ImVec2(track_x + track_w, pos.y + size.y),
                      track_bg, 4.0f);
    dl->AddRectFilled(ImVec2(track_x + 1, pos.y + 1),
                      ImVec2(track_x + track_w - 1, pos.y + size.y - 1),
                      track_inner, 3.0f);

    // Filled portion below handle
    float t          = 1.0f - (*value - min) / (max - min);
    float handle_y   = pos.y + t * size.y;
    float fill_top   = handle_y;
    ImU32 fill_col   = IM_COL32(0, 210, 200, 180);
    dl->AddRectFilled(ImVec2(track_x + 1, fill_top),
                      ImVec2(track_x + track_w - 1, pos.y + size.y - 2),
                      fill_col, 3.0f);

    // Handle
    const float hw   = size.x * 0.72f;
    const float hh   = 28.0f;
    ImVec2 hmin      = ImVec2(pos.x + (size.x - hw) * 0.5f, handle_y - hh * 0.5f);
    ImVec2 hmax      = ImVec2(hmin.x + hw, hmin.y + hh);

    // Shadow
    dl->AddRectFilled(ImVec2(hmin.x + 3, hmin.y + 4),
                      ImVec2(hmax.x + 3, hmax.y + 4),
                      IM_COL32(0, 0, 0, 100), 8.0f);

    // Base
    ImU32 base_col = active  ? IM_COL32(50, 220, 210, 255)
                   : hovered ? IM_COL32(40, 200, 190, 255)
                             : IM_COL32(30, 180, 170, 255);
    dl->AddRectFilled(hmin, hmax, base_col, 8.0f);

    // Bottom dark edge (gives depth)
    dl->AddRectFilled(ImVec2(hmin.x + 2, hmax.y - 5),
                      ImVec2(hmax.x - 2, hmax.y - 1),
                      IM_COL32(0, 80, 75, 200), 4.0f);

    // Top highlight (raised light edge)
    dl->AddRectFilled(ImVec2(hmin.x + 2, hmin.y + 1),
                      ImVec2(hmax.x - 2, hmin.y + 5),
                      IM_COL32(120, 255, 245, 180), 4.0f);

    // Left highlight
    dl->AddRectFilled(ImVec2(hmin.x + 1, hmin.y + 4),
                      ImVec2(hmin.x + 4, hmax.y - 4),
                      IM_COL32(100, 240, 230, 120), 2.0f);

    // Grip lines
    ImU32 grip_light = IM_COL32(180, 255, 250, 160);
    ImU32 grip_dark  = IM_COL32(0,  60,  55, 180);
    float cx         = hmin.x + hw * 0.5f;
    float cy         = hmin.y + hh * 0.5f;
    for (int g = -1; g <= 1; g++)
    {
        float gy = cy + g * 6.0f;
        float gw = hw * 0.45f;
        dl->AddLine(ImVec2(cx - gw, gy + 1), ImVec2(cx + gw, gy + 1), grip_dark,  1.5f);
        dl->AddLine(ImVec2(cx - gw, gy),     ImVec2(cx + gw, gy),     grip_light, 1.0f);
    }

    // Border
    dl->AddRect(hmin, hmax, IM_COL32(0, 120, 110, 255), 8.0f, 0, 1.5f);

    if (hovered || active)
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);

    return changed;
}