#include <iostream>
#include "Window.h"
#include "Attractor.h"

void redevelop(Attractor& a, GLuint tex) {
    a.develop();

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a.map.width(), a.map.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, &a.image.array[0]);
}
int main() {
    std::cout << "Hello, World!" << std::endl;

    Window window {};

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same


    Attractor attractor {};
    attractor.iterate();
    redevelop(attractor, image_texture);

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
            float f;
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

            ImGui::End();
        }

        window.render();
    }
    return 0;
}
