#pragma once
#include <string>
#include <memory>
#include <vector>

// Forward declare GLFWwindow to avoid including GLFW here
struct GLFWwindow;

class UIComponent;

class Application
{
public:
    Application(int width, int height, const std::string& title);
    ~Application();

    bool Initialize();
    void Run();
    void AddComponent(std::shared_ptr<UIComponent> component);

    GLFWwindow* GetWindow() const { return window; }

private:
    bool InitializeGLFW();
    bool InitializeImGui();
    void Shutdown();
    void BeginFrame();
    void EndFrame();

    GLFWwindow* window;
    int width;
    int height;
    std::string title;
    std::vector<std::shared_ptr<UIComponent>> components;
};