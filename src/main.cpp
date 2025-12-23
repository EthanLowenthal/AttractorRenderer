#define GL_SILENCE_DEPRECATION

#include "window.h"
#include "clifford.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>


int main(int argc, char* argv[])
{
    std::atomic<float> progress = 0.0f;

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
                ImGui::ColorEdit3("Background Color", &config.bg_color[0]);
                ImGui::Text("Palette:");
                for (int i = 0; i < config.palette.colors.size(); i++) {
                    ImGui::PushID(i);
                    ImGui::ColorEdit3("", &config.palette.colors[i].second[0]);
                    ImGui::PopID();
                    ImGui::SameLine();
                    ImGui::PushID(i + 1000);
                    ImGui::SliderFloat("", &config.palette.colors[i].first, 0.0f, 1.0f);
                    ImGui::PopID();
                    ImGui::SameLine();
                    ImGui::PushID(i + 2000);
                    if (ImGui::Button("Remove")) {
                        config.palette.colors.erase(config.palette.colors.begin() + i);
                    }
                    ImGui::PopID();
                }
                if (ImGui::Button("Add Color")) {
                    config.palette.addColor(0.5f, 1.0f, 1.0f, 1.0f);
                }
                ImGui::Text("Preview:");
                ImGui::BeginChild("PalettePreview", ImVec2(0, 30), true);
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
                ImVec2 canvas_size = ImGui::GetContentRegionAvail();
                for (int i = 0; i < canvas_size.x; i++) {
                    float t = i / canvas_size.x;
                    auto color = config.palette.getColor(t);
                    ImU32 col = IM_COL32((int)(color[0] * 255), (int)(color[1] * 255), (int)(color[2] * 255), 255);
                    draw_list->AddLine(ImVec2(canvas_pos.x + i, canvas_pos.y), ImVec2(canvas_pos.x + i, canvas_pos.y + canvas_size.y), col);
                }
                ImGui::EndChild();
            }

            if (ImGui::CollapsingHeader("Other")) {
                ImGui::SliderInt("Number of Points", &config.num_points, 10, 100000);
                ImGui::SliderInt("Iterations per Point", &config.num_iterations, 1000, 1000000);
            }
            
            if (ImGui::CollapsingHeader("Generate Image")) {
                if (ImGui::Button("Generate")) {
                    std::thread([&config, &progress]() {
                        generate_clifford_image(config, progress);
                    }).detach();
                }
                if (progress > 0.0f) {
                    ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f));
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
