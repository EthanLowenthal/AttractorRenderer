#include <iostream>
#include "Window.h"
#include "Attractor.h"
#include "Palette.h"

void regen_palette(Attractor& a, GLuint tex) {
    int width = 400;

    auto palette_img = gen_palette_img(a.palette, width);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &palette_img[0]);
}

void redevelop(Attractor& a, GLuint tex) {
    a.develop();

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a.map.width(), a.map.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, &a.image.array[0]);
}
int main() {
    Window window {};

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    GLuint palette_texture;
    glGenTextures(1, &palette_texture);
    glBindTexture(GL_TEXTURE_2D, palette_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    Attractor attractor {};
    attractor.iterate();
    redevelop(attractor, image_texture);

    regen_palette(attractor, palette_texture);

    bool done = false;

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
            ImGui::Image((void*)(intptr_t)image_texture, ImVec2(attractor.map.width(), attractor.map.height()));
            ImGui::End();
        }
        {
            ImGui::Begin("Image Settings");

            ImGui::InputInt("iterations", &attractor.iterations);
            ImGui::SameLine();
            if (ImGui::Button("Iterate")) {
                attractor.iterate();
                redevelop(attractor, image_texture);
            }
            ImGui::SameLine();
            if (ImGui::Button("Develop")) {
                redevelop(attractor, image_texture);
            }

            ImGui::Separator();

            if(ImGui::CollapsingHeader("Palette")) {
                ImGui::Image((void*)(intptr_t)palette_texture, ImVec2(400, 100));
            }

            if(ImGui::CollapsingHeader("Mapping")) {
                ImGui::Text("map");
            }

            if(ImGui::CollapsingHeader("Save")) {
                ImGui::Text("save");
            }

            ImGui::End();
        }

        window.render();
    }
    return 0;
}
