#include <core/Application.h>
#include <core/UIComponent.h>

// CRITICAL: glad MUST come before GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Then ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

Application::Application(int width, int height, const std::string& title)
    : window(nullptr), width(width), height(height), title(title)
{
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    if (!InitializeGLFW())
        return false;
    
    if (!InitializeImGui())
        return false;

    return true;
}

bool Application::InitializeGLFW()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glViewport(0, 0, width, height);
    
    return true;
}

bool Application::InitializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    return true;
}

void Application::AddComponent(std::shared_ptr<UIComponent> component)
{
    components.push_back(component);
}

void Application::BeginFrame()
{
    glfwPollEvents();
    
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Application::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

void Application::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        BeginFrame();
        
        // Render all components
        for (auto& component : components)
        {
            component->Render();
        }
        
        EndFrame();
    }
}

void Application::Shutdown()
{
    components.clear();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    
    glfwTerminate();
}