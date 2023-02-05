
#include "imgui.h"
#include <cmath>
#include "log.h"
#include "colors.h"
#include "server_report_store.h"
#include "connection.h"
#include <stdlib.h>
#include "customwidgets.h"

// window.open(url, '_blank').focus();

void about_view_frame()
{
    ImGui::Text("Npanel , a UI implementation for API of the great Trojan-go , created by :");
    ImGui::SameLine();
    if (ImGui::SmallButton("p4gefau1t"))
    {
        EM_ASM(window.open("https://github.com/p4gefau1t/trojan-go", '_blank').focus(););
    }

    ImGui::Text("And we use the special fork made by :");
    ImGui::SameLine();
    if (ImGui::SmallButton("gfw_report"))
    {
        EM_ASM(window.open("https://github.com/gfw-report/trojan-go", '_blank').focus(););
    }
    ImGui::Separator();
    ImGui::Text("Contact Author for any reasons (bugs, helps, suggestions etc..)");
    ImGui::SameLine();
    if (ImGui::SmallButton("Leiren"))
    {
        EM_ASM(window.open("https://t.me/tired_mans", '_blank').focus(););
    }
    ImGui::SameLine();
    ImGui::Text("pm friendly :)");
    ImGui::Text("If you like this panel, consider buying me a coffee! thanks.");

    ImGui::Text("If you find this panel useful please");
    ImGui::SameLine();
    if (ImGui::SmallButton("star me on github"))
    {
        EM_ASM(window.open("https://github.com/Leiren/Npanel", '_blank').focus(););
    }
    ImGui::SameLine();
    ImGui::Text("; i really appreciate that,Thanks.");

    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    auto content_avail = ImGui::GetContentRegionAvail();
    ImGui::Dummy(ImVec2(0, content_avail.y - TEXT_BASE_HEIGHT * 1));
    ImGui::Text("made with");
    ImGui::SameLine();
    if (ImGui::SmallButton("Dear ImGui"))
    {
        EM_ASM(window.open("https://github.com/ocornut/imgui", '_blank').focus(););
    }
}