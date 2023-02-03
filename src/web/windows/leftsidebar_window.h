#pragma once
#include "window.h"
#include "rocket.hpp"

class LeftSideBarWindow : public Window
{
private:
    /* data */
    static int selected_index;

public:
    static rocket::signal<void(int)> on_tab_changed;

    LeftSideBarWindow(/* args */) : Window("Options")
    {
        hideCloseButton();
        dock(DockDir::left);
    }

    virtual void onFrame() override;

    virtual int onStyle() override
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 5.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetFontSize(), 15.0f));

        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        ImGui::SetNextWindowClass(&window_class);
        return 4;
    }
};
