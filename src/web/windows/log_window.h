#pragma once
#include "imgui.h"
#include "window.h"

class LogWindow : public Window
{
private:
    /* data */
public:


    LogWindow(/* args */) : Window("Logs")
    {
        hideCloseButton();
        dock(DockDir::down);
    }

    virtual void onFrame() override;

    virtual int onStyle() override
    {
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 5.0f));
        // ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetFontSize(), 8.0f));

        ImGuiWindowClass window_class;
        window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
        ImGui::SetNextWindowClass(&window_class);
        return 0;
    }
 
};
