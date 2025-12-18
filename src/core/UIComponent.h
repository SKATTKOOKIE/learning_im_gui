#pragma once

struct GLFWwindow;

class UIComponent
{
public:
    virtual ~UIComponent() = default;
    virtual void Render() = 0;

    void SetWindow(GLFWwindow *win) { window = win; }

protected:
    GLFWwindow *window = nullptr;
};