
#include "imgui.h"
#include "log.h"
#include "connection.h"

extern void apply_theme();

void setup()
{
    ImGui::GetIO().IniFilename = nullptr;
    apply_theme();
    Connection::init();
    // ApplicatoinBackgorundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // ImGuiIO &io = ImGui::GetIO();
    // io.Fonts->AddFontFromFileTTF("Assets/OpenSans-Bold.ttf", 16.0f);
}