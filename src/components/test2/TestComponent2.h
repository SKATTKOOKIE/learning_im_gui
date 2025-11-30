#pragma once
#include "core/UIComponent.h"

class TestComponent2 : public UIComponent
{
public:
    TestComponent2();
    void Render() override;

private:
    bool show_example_text;
};