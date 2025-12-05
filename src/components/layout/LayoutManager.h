#pragma once
#include <core/UIComponent.h>
#include <imgui.h>
#include <map>
#include <memory>
#include <vector>

enum class DockZone
{
    Left,
    Right,
    Top,
    Bottom,
    Center
};

class LayoutManager : public UIComponent
{
public:
    LayoutManager();
    void Render() override;
    
    // Add a widget to a specific zone
    void AddWidget(DockZone zone, std::shared_ptr<UIComponent> widget);
    
private:
    void SetupDockingLayout();
    ImGuiID GetDockIDForZone(DockZone zone);
    
    bool first_frame;
    ImGuiID dockspace_id;
    std::map<DockZone, ImGuiID> dock_zones;
    std::map<DockZone, std::vector<std::shared_ptr<UIComponent>>> widgets;
};