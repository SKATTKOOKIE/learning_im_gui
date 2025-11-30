#pragma once
#include "core/UIComponent.h"

class TestComponent : public UIComponent
{
public:
    TestComponent();
    void Render() override;

private:
    bool show_example_text;
};