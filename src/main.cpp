#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include "window.h"

struct ImageConfig {
    int width = 500, height = 500;
    float scale;
};

struct CliffordConfig: public ImageConfig  {
    float a, b, c, d;
    int num_points;
};

int main(int argc, char* argv[])
{
    const char* glsl_version = "#version 150";
    GLFWwindow* window = initGLFW(glsl_version);
    if (window == NULL)
        return 1;

    ImGuiIO& io = initImGui(window, glsl_version);

    CliffordConfig config = {};
    
    while (!glfwWindowShouldClose(window))
    {
        startFrame();

        {
            static float f = 0.0f;
            static int counter_val = 0;

            ImGui::SetNextWindowPos(ImVec2(0,0));
            ImGui::SetNextWindowSize(io.DisplaySize);
            

            ImGui::Begin(
                "Main Window",
                NULL,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus
            );

            if (ImGui::CollapsingHeader("Clifford Attractor")) {
                ImGui::SliderFloat("a", &config.a, -2.0f, 2.0f);
                ImGui::SliderFloat("b", &config.b, -2.0f, 2.0f);
                ImGui::SliderFloat("c", &config.c, -2.0f, 2.0f);
                ImGui::SliderFloat("d", &config.d, -2.0f, 2.0f);
            }

            if (ImGui::CollapsingHeader("Image Configuration")) {
                ImGui::InputInt("Width", &config.width);
                ImGui::InputInt("Height", &config.height, 100, 1000);
                ImGui::SliderFloat("Scale", &config.scale, 0.1f, 10.0f);
            }

            if (ImGui::CollapsingHeader("Other")) {
                ImGui::SliderInt("Number of Points", &config.num_points, 1000, 1000000);
            }

            if (ImGui::CollapsingHeader("Generate Image")) {
                if (ImGui::Button("Generate")) {
                    std::cout << "Generating image with configuration:" << std::endl;
                    std::cout << "a = " << config.a << ", b = " << config.b << ", c = " << config.c << ", d = " << config.d << std::endl;
                    std::cout << "Width = " << config.width << ", Height = " << config.height << ", Scale = " << config.scale << std::endl;
                }
            }
   
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        finishFrame(window);
    }

    cleanupImGui();
    cleanupGLFW(window);

    return 0;
}
