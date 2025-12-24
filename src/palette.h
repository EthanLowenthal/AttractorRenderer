#pragma once

#include "imgui.h"
#include "imgui_internal.h"

struct PaletteHandle
{
    float t;
    ImVec4 color;
};

struct Palette {
    ImVector<PaletteHandle> palette {};
    ImVector<PaletteHandle*> palette_handles {};

    Palette() {
        addColor(0.1, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        addColor(0.5, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        addColor(1.0, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    void addColor(float position, ImVec4 color) {
        palette.push_back( { position, color });
        palette_handles.push_back(&palette.back());
        sort();
    }
    void sort() {
        std::sort(palette_handles.begin(), palette_handles.end(), [](const auto& a, const auto& b) {
            return a->t < b->t;
        });
    }
    ImVec4 getColor(const float position) const {
        if (palette_handles.empty()) return ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

        if (position <= palette_handles.front()->t) return palette_handles.front()->color;
        if (position >= palette_handles.back()->t) return palette_handles.back()->color;

        for (int i = 0; i < palette_handles.size() - 1; i++) {
            if (position >= palette_handles[i]->t && position <= palette_handles[i + 1]->t) {
                const float t = (position - palette_handles[i]->t) / (palette_handles[i + 1]->t - palette_handles[i]->t);

                return {
                    palette_handles[i]->color.x * (1.0f - t) + palette_handles[i + 1]->color.x * t,
                    palette_handles[i]->color.y * (1.0f - t) + palette_handles[i + 1]->color.y * t,
                    palette_handles[i]->color.z * (1.0f - t) + palette_handles[i + 1]->color.z * t,
                    palette_handles[i]->color.w * (1.0f - t) + palette_handles[i + 1]->color.w * t,
                };
            }
        }
        return palette_handles.back()->color;
    }
};

bool MultiColorSlider(
    const char* label,
    Palette& palette,
    float width = 300.0f,
    float height = 20.0f)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGui::PushID(label);

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 size(width, height);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::InvisibleButton("##slider", size);
    bool changed = false;
    static PaletteHandle* selected = nullptr;

    ImDrawList* draw = ImGui::GetWindowDrawList();

    draw->AddRectFilled(bb.Min, bb.Max, IM_COL32(40, 40, 40, 255), 4.0f);


    for (int i = 0; i < palette.palette_handles.Size - 1; ++i)
    {
        float x0 = ImLerp(bb.Min.x, bb.Max.x, palette.palette_handles[i]->t);
        float x1 = ImLerp(bb.Min.x, bb.Max.x, palette.palette_handles[i + 1]->t);

        draw->AddRectFilledMultiColor(
            ImVec2(x0, bb.Min.y),
            ImVec2(x1, bb.Max.y),
            ImGui::ColorConvertFloat4ToU32(palette.palette_handles[i]->color),
            ImGui::ColorConvertFloat4ToU32(palette.palette_handles[i + 1]->color),
            ImGui::ColorConvertFloat4ToU32(palette.palette_handles[i + 1]->color),
            ImGui::ColorConvertFloat4ToU32(palette.palette_handles[i]->color)
        );
    }

    if (!palette.palette_handles.empty()) {
        float x = ImLerp(bb.Min.x, bb.Max.x, palette.palette_handles.front()->t);
        draw->AddRectFilled(ImVec2(bb.Min.x, bb.Min.y), ImVec2(x, bb.Max.y), ImGui::ColorConvertFloat4ToU32(palette.palette_handles.front()->color));

        x = ImLerp(bb.Min.x, bb.Max.x, palette.palette_handles.back()->t);
        draw->AddRectFilled(ImVec2(x, bb.Min.y), ImVec2(bb.Max.x, bb.Max.y), ImGui::ColorConvertFloat4ToU32(palette.palette_handles.back()->color));
    }

    ImVec2 mouse = ImGui::GetIO().MousePos;

    if (!ImGui::IsMouseDown(0))
        selected = nullptr;

    if (selected) {
        float new_t = (mouse.x - bb.Min.x) / (bb.Max.x - bb.Min.x);

        if (new_t > 0 && new_t < 1)
            selected->t = new_t;

        changed = true;
    }

    for (int i = 0; i < palette.palette_handles.Size; ++i)
    {
        float x = ImLerp(bb.Min.x, bb.Max.x, palette.palette_handles[i]->t);
        ImVec2 handle_pos(x, bb.GetCenter().y);

        ImVec2 radius = {6.0f, 12.0f};

        ImRect handle_bb(
            handle_pos.x - radius.x, handle_pos.y - radius.y,
            handle_pos.x + radius.x, handle_pos.y + radius.y
        );

        bool hovered = handle_bb.Contains(mouse);

        if (hovered && ImGui::IsMouseClicked(0)) {
            ImGui::SetActiveID(window->GetID(i), window);
            if (selected == nullptr) {
                selected = palette.palette_handles[i];
            }
        }


        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4 buttonColor = style.Colors[ImGuiCol_Button];
        ImVec4 buttonHoveredColor = style.Colors[ImGuiCol_ButtonHovered];
        ImVec4 buttonActiveColor = style.Colors[ImGuiCol_ButtonActive];

        float border = 3.0f;
        draw->AddRectFilled(
            ImVec2(handle_bb.Min.x - border, handle_bb.Min.y - border),
            ImVec2(handle_bb.Max.x + border, handle_bb.Max.y + border),
            ImGui::ColorConvertFloat4ToU32(selected == palette.palette_handles[i] ? buttonActiveColor : (hovered ? buttonHoveredColor : buttonColor))
        );

        draw->AddRectFilled(handle_bb.Min, handle_bb.Max, ImGui::ColorConvertFloat4ToU32(palette.palette_handles[i]->color));
    }

    ImGui::PopID();
    ImGui::TextUnformatted(label);

    if (changed) {
        palette.sort();
    }

    return changed;
}
