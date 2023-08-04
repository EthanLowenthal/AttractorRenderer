#include <iostream>
#include <thread>

#include "Window.h"
#include "misc/cpp/imgui_stdlib.h"

#include "Attractor.h"
#include "Palette.h"
#include "libs/ImGuiFileDialog/ImGuiFileDialog.h"

void regen_palette(Attractor& a, GLuint tex) {
    int width = 400;

    auto palette_img = gen_palette_img(a.palette, width);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &palette_img[0]);
}

void redevelop(Attractor& a, GLuint tex) {
    a.develop();

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a.size.x, a.size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &a.image.array[0]);
}
int main() {
    Window window {};

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLuint palette_texture;
    glGenTextures(1, &palette_texture);
    glBindTexture(GL_TEXTURE_2D, palette_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int progress {0};

    Attractor attractor {};
    attractor.iterate(progress);
    redevelop(attractor, image_texture);

    regen_palette(attractor, palette_texture);

    bool done = false;

    bool lock_palette[4] = {true, true, true, true};
    std::string filename {"out"};


    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window.window))
                done = true;
        }

        window.new_frame();
        {
            ImGui::Begin("Image");

            vec2<float> rendered_bounds_size = {
                    attractor.rendered_bounds.x[1] - attractor.rendered_bounds.x[0],
                    attractor.rendered_bounds.y[1] - attractor.rendered_bounds.y[0]
            };
            vec2<ImVec2> uv = {
            {
                    (attractor.bounds.x[0] - attractor.rendered_bounds.x[0]) / rendered_bounds_size.x,
                    (attractor.bounds.y[0] - attractor.rendered_bounds.y[0]) / rendered_bounds_size.y
                },
            {
                    (attractor.bounds.x[1] - attractor.rendered_bounds.x[0]) / rendered_bounds_size.x,
                    (attractor.bounds.y[1] - attractor.rendered_bounds.y[0]) / rendered_bounds_size.y
                },
            };

            ImGui::Image((void*)(intptr_t)image_texture, ImVec2(attractor.size.x, attractor.size.y), uv[0], uv[1]);
            ImGui::End();
        }
        {
            ImGui::Begin("Image Settings");

            ImGui::InputInt("iterations", &attractor.iterations);
            ImGui::SameLine();
            if (ImGui::Button("Iterate")) {
                progress = 0;
                std::thread iterate_thread (&Attractor::iterate, std::ref(attractor), std::ref(progress));
                iterate_thread.detach();
            }
            ImGui::SameLine();
            if (ImGui::Button("Develop")) {
                redevelop(attractor, image_texture);
            }

            ImGui::ProgressBar(static_cast<float>(progress)/attractor.iterations);
            ImGui::Separator();

            if(ImGui::CollapsingHeader("Palette")) {
                ImGui::Image((void *) (intptr_t) palette_texture, ImVec2(400, 100));


                for (int i=0;i<4;i++) {
                    for (int j=0;j<3;j++) {
                        ImGui::PushID(j*10+i);
                        if (ImGui::VSliderFloat("##v", ImVec2(18, 100), &attractor.palette[i][j], 0, 5, "")) {
                            if (lock_palette[i]) {
                                attractor.palette[i].x = attractor.palette[i][j];
                                attractor.palette[i].y = attractor.palette[i][j];
                                attractor.palette[i].z = attractor.palette[i][j];
                            }
                            regen_palette(attractor, palette_texture);
                        }
                        ImGui::PopID();
                        ImGui::SameLine();
                    }
                    ImGui::PushID(i*100);
                    ImGui::Checkbox("lock", &lock_palette[i]);
                    ImGui::PopID();
                    ImGui::SameLine();

                }
                ImGui::NewLine();
            }

            if(ImGui::CollapsingHeader("Map")) {

                ImVec2 start = ImGui::GetCursorPos();

                ImGui::Image((void *) (intptr_t) palette_texture, ImVec2(400, 100));

                ImGui::SetCursorPos(start);

                ImGui::PlotHistogram("Histogram", &attractor.histogram.array[0], attractor.histogram.width(), 0, "", 0, attractor.histogram.max(), {400, 100});

                if(ImGui::SliderFloat("gamma", &attractor.gamma, 0.0, 15.0))
                    redevelop(attractor, image_texture);
                if(ImGui::SliderFloat("exposure", &attractor.exposure, 0.0, 1.0))
                    redevelop(attractor, image_texture);
                if(ImGui::SliderFloat("base", &attractor.base, 0.0, 15.0))
                    redevelop(attractor, image_texture);


                ImGui::Text("Bounds");
                if(ImGui::SliderFloat2("x", &attractor.bounds.x[0], -5.0, 5.0));
                if(ImGui::SliderFloat2("y", &attractor.bounds.y[0], -5.0, 5.0));

                ImGui::Text("Params");
                if(ImGui::SliderFloat("a", &attractor.a, -2.0, 2.0));
                if(ImGui::SliderFloat("b", &attractor.b, -2.0, 2.0));
                if(ImGui::SliderFloat("c", &attractor.c, -2.0, 2.0));
                if(ImGui::SliderFloat("d", &attractor.d, -2.0, 2.0));

//                    redevelop(attractor, image_texture);
            }

            if(ImGui::CollapsingHeader("Image")) {


                if(ImGui::InputInt("supersample", &attractor.supersample));
                if(ImGui::InputInt2("image size", &attractor.size[0]));

//                    redevelop(attractor, image_texture);
            }


            if(ImGui::CollapsingHeader("Save")) {
                ImGui::InputText(".png", &filename);
                ImGui::SameLine();
                if(ImGui::Button("Write")) {
                    redevelop(attractor, image_texture);
                    attractor.save(filename);
                }

                if (ImGui::Button("Open File"))
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".png", ".");

                if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
                {
                    if (ImGuiFileDialog::Instance()->IsOk())
                    {
                        std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                        std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
                        attractor.load(filePathName);
                    }

                    ImGuiFileDialog::Instance()->Close();
                }

            }

            ImGui::End();
        }

        window.render();
    }
    return 0;
}
