#pragma once

class UIComponent
{
public:
    virtual ~UIComponent() = default;
    virtual void Render() = 0;
};