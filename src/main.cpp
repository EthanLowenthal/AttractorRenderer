#define GL_SILENCE_DEPRECATION

#include "window.h"
#include "clifford.h"
#include "palette.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <algorithm>
#include <atomic>




int main(int argc, char* argv[])
{
    std::atomic<float> progress = 0.0f;

    const char* glsl_version = "#version 150";
    GLFWwindow* window = initGLFW(glsl_version);
    if (window == NULL)
        return 1;

    ImGuiIO& io = initImGui(window, glsl_version);

    CliffordAttractor attractor = {};
    
    while (!glfwWindowShouldClose(window))
    {

        if (attractor.dirty) {
            attractor.generate_image();
        }

        startFrame();

        {
            ImGui::SetNextWindowPos(ImVec2(0,0));
            ImGui::SetNextWindowSize(io.DisplaySize);
            

            ImGui::Begin(
                "Main Window",
                NULL,
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus
            );

            if (ImGui::BeginTable("Table", 2)) {
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 400.0f);
                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch, 1.0f);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                if (ImGui::CollapsingHeader("Clifford Attractor")) {
                    ImGui::SliderFloat("a", &attractor.a, -2.0f, 2.0f);
                    ImGui::SliderFloat("b", &attractor.b, -2.0f, 2.0f);
                    ImGui::SliderFloat("c", &attractor.c, -2.0f, 2.0f);
                    ImGui::SliderFloat("d", &attractor.d, -2.0f, 2.0f);
                }

                if (ImGui::CollapsingHeader("Image Configuration")) {
                    ImGui::InputInt("Width", &attractor.width);
                    ImGui::InputInt("Height", &attractor.height, 100, 1000);
                    ImGui::SliderFloat("Scale", &attractor.scale, 0.1f, 10.0f);
                    ImGui::ColorEdit3("Background Color", &attractor.bg_color[0], ImGuiColorEditFlags_NoInputs);
                    ImGui::Text("Palette:");
                    MultiColorSlider("", attractor.palette);
                    // for (int i = 0; i < attractor.palette.colors.size(); i++) {
                    //     ImGui::PushID(i);
                    //     ImGui::ColorEdit3("", &attractor.palette.colors[i].second[0], ImGuiColorEditFlags_NoInputs);
                    //     ImGui::SameLine();
                    //     ImGui::SliderFloat("", &attractor.palette.colors[i].first, 0.0f, 1.0f);
                    //     ImGui::SameLine();
                    //     if (ImGui::Button("Remove")) {
                    //         attractor.palette.colors.erase(attractor.palette.colors.begin() + i);
                    //     }
                    //     ImGui::PopID();
                    // }
                    // if (ImGui::Button("Add Color")) {
                    //     attractor.palette.addColor(0.5f, 1.0f, 1.0f, 1.0f);
                    // }
                    // ImGui::Text("Preview:");
                    // ImGui::BeginChild("PalettePreview", ImVec2(0, 30), true);
                    // ImDrawList* draw_list = ImGui::GetWindowDrawList();
                    // ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
                    // ImVec2 canvas_size = ImGui::GetContentRegionAvail();
                    // for (int i = 0; i < canvas_size.x; i++) {
                    //     float t = i / canvas_size.x;
                    //     auto color = attractor.palette.getColor(t);
                    //     ImU32 col = IM_COL32((int)(color[0] * 255), (int)(color[1] * 255), (int)(color[2] * 255), 255);
                    //     draw_list->AddLine(ImVec2(canvas_pos.x + i, canvas_pos.y), ImVec2(canvas_pos.x + i, canvas_pos.y + canvas_size.y), col);
                    // }
                    // ImGui::EndChild();
                }

                if (ImGui::CollapsingHeader("Other")) {
                    ImGui::SliderInt("Number of Points", &attractor.num_points, 10, 100000);
                    ImGui::SliderInt("Iterations per Point", &attractor.num_iterations, 1000, 100000);
                }

                if (ImGui::Button("Generate")) {
                    std::thread([&]() {
                        attractor.generate_hits(progress);
                    }).detach();
                }
                ImGui::SameLine();
                if (ImGui::Button("Update")) {
                    attractor.generate_image();
                }
                if (progress > 0.0f) {
                    ImGui::ProgressBar(progress, ImVec2(-1.0f, 0.0f));
                }

                ImGui::TableSetColumnIndex(1);

                if (attractor.image_texture) {
                    ImGui::Image((ImTextureID)(intptr_t)attractor.image_texture, ImVec2(attractor.image_width, attractor.image_height));
                }

                ImGui::EndTable();
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
