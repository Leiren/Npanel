#include "imgui.h"
#include "imnodes.h"
#include "server_report_store.h"
#include <cmath>
#include "log.h"
#include "colors.h"

void tunnels_view_frame()
{
    static bool first_time = true;
    if (first_time)
    {

        ImNodes::SetNodeGridSpacePos(1, ImVec2(200.0f, 200.0f));
    }
    ImGui::Text("work in progress...");
    ImGui::Separator();
    ImNodes::BeginNodeEditor();
    if (first_time)
        ImNodes::SetNodeScreenSpacePos(1, ImVec2(400, 200));
    ImNodes::BeginNode(1);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("This Server");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(21);
    ImGui::Text("Self");
    ImNodes::EndInputAttribute();

    ImNodes::BeginInputAttribute(20);
    ImGui::Text("User Auth");
    ImNodes::EndInputAttribute();
    ImNodes::BeginInputAttribute(2);
    ImGui::Text("443");
    ImNodes::EndInputAttribute();
    ImNodes::BeginOutputAttribute(3);
    ImGui::Indent(80);
    ImGui::Text("output(NAT)");
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();
    if (first_time)
        ImNodes::SetNodeScreenSpacePos(4, ImVec2(800, 350));
    ImNodes::BeginNode(4);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Internet");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(5);
    ImGui::Text("input");
    ImNodes::EndInputAttribute();

    ImNodes::EndNode();

    ImNodes::Link(6, 3, 5);
    ImNodes::Link(200, 21, 20);

    ImNodes::EndNodeEditor();
    first_time = false;
}