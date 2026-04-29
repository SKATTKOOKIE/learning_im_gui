#include "components/layout/CenterColumnWidget.h"
#include <imgui.h>

CenterColumnWidget::CenterColumnWidget(std::shared_ptr<TestVerticalSlider> slider,
                                       std::shared_ptr<JointStatusTable>   table)
    : slider_(std::move(slider))
    , table_(std::move(table))
{}

void CenterColumnWidget::Render()
{
    slider_->Render();
    table_->Render();
}