#pragma once

#include "window.h"
#include "leftsidebar_window.h"

class MainWindow : public Window
{
private:
    /* data */
    int running_index;

public:
    MainWindow(/* args */) : Window("Main Panel"), running_index(-1)
    {
        hideCloseButton();
        dock(DockDir::main);
    }

    virtual void onFrame() override;

    virtual int onStyle() override
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 7.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);

        // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 5.0f));
        // ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetFontSize(), 8.0f));

        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        ImGui::SetNextWindowClass(&window_class);
        return 5;
    }
};
