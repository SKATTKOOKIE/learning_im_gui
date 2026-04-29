#pragma once
#include "core/UIComponent.h"
#include "components/TestVerticalSlider/TestVerticalSlider.h"
#include "components/JointStatusTable/JointStatusTable.h"
#include <memory>

class CenterColumnWidget : public UIComponent
{
public:
    CenterColumnWidget(std::shared_ptr<TestVerticalSlider> slider,
                       std::shared_ptr<JointStatusTable>   table);
    void Render() override;

    std::shared_ptr<JointStatusTable> GetTable() { return table_; }

private:
    std::shared_ptr<TestVerticalSlider> slider_;
    std::shared_ptr<JointStatusTable>   table_;
};