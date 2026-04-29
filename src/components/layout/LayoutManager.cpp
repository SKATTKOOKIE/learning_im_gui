#include "components/layout/LayoutManager.h"
#include <imgui.h>
#include <imgui_internal.h>

LayoutManager::LayoutManager()
    : first_frame(true), dockspace_id(0)
{
}

void LayoutManager::SetupDockingLayout()
{
    // Only setup the layout on the first frame
    if (!first_frame)
        return;

    first_frame = false;

    // Clear any existing layout
    ImGui::DockBuilderRemoveNode(dockspace_id);
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

    // Split the dockspace into custom zones matching your layout
    ImGuiID left, right, top, bottom, center;
    ImGuiID remaining = dockspace_id;
    
    
    // First split off left panel (15% width)
    left = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Left, 0.20f, nullptr, &remaining);
    
    // Then split off right panel (15% width of remaining)
    right = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Right, 0.176f, nullptr, &remaining);
    
    // Split top panel (10% height of remaining)
    top = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Up, 0.10f, nullptr, &remaining);
    
    // Split bottom panel (10% height of remaining)
    bottom = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Down, 0.111f, nullptr, &center);

    // Store the dock IDs
    dock_zones[DockZone::Left] = left;
    dock_zones[DockZone::Right] = right;
    dock_zones[DockZone::Top] = top;
    dock_zones[DockZone::Bottom] = bottom;
    dock_zones[DockZone::Center] = center;

    // Set node flags to prevent splitting in restricted zones
    ImGui::DockBuilderGetNode(left)->LocalFlags |= ImGuiDockNodeFlags_NoSplit;
    ImGui::DockBuilderGetNode(right)->LocalFlags |= ImGuiDockNodeFlags_NoSplit;
    ImGui::DockBuilderGetNode(top)->LocalFlags |= ImGuiDockNodeFlags_NoSplit;
    ImGui::DockBuilderGetNode(bottom)->LocalFlags |= ImGuiDockNodeFlags_NoSplit;
    ImGui::DockBuilderGetNode(center)->LocalFlags |= ImGuiDockNodeFlags_NoSplit;
    
    ImGui::DockBuilderGetNode(left)->LocalFlags   |= ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoTabBar;
    ImGui::DockBuilderGetNode(right)->LocalFlags  |= ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoTabBar;
    ImGui::DockBuilderGetNode(top)->LocalFlags    |= ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoTabBar;
    ImGui::DockBuilderGetNode(bottom)->LocalFlags |= ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoTabBar;
    ImGui::DockBuilderGetNode(center)->LocalFlags |= ImGuiDockNodeFlags_NoUndocking | ImGuiDockNodeFlags_NoTabBar;

    ImGui::SetNextWindowSizeConstraints({ 350.0f, -1.0f }, { 400.0f, -1.0f });

    ImGui::DockBuilderFinish(dockspace_id);
}

ImGuiID LayoutManager::GetDockIDForZone(DockZone zone)
{
    auto it = dock_zones.find(zone);
    if (it != dock_zones.end())
        return it->second;
    return dockspace_id;
}

void LayoutManager::AddWidget(DockZone zone, std::shared_ptr<UIComponent> widget)
{
    widget->SetWindow(window);
    widgets[zone].push_back(widget);
}

void LayoutManager::Render()
{
    // Create a fullscreen dockspace
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();

    // Setup the custom docking layout
    SetupDockingLayout();

    // Render all widgets in their respective zones
    for (auto &[zone, widget_list] : widgets)
    {
        ImGuiID dock_id = GetDockIDForZone(zone);
        
        for (auto &widget : widget_list)
        {
            // Dock the window to the appropriate zone on first appearance
            // ImGui::SetNextWindowDockID(dock_id, ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowDockID(dock_id, ImGuiCond_Always);
            widget->Render();
        }
    }
}

void LayoutManager::SetWindow(GLFWwindow* win)
{
    window = win;
    for (auto& [zone, widget_list] : widgets)
        for (auto& widget : widget_list)
            widget->SetWindow(win);
}