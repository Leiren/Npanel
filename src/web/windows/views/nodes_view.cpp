#include "imgui.h"
#include "imnodes.h"
#include "server_report_store.h"
#include <cmath>
#include "log.h"
#include "colors.h"

void nodes_view_frame()
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
        ImNodes::SetNodeScreenSpacePos(1, ImVec2(500, 200));
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
    ImGui::Indent(200);
    ImGui::Text("output");
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();
    if (first_time)
        ImNodes::SetNodeScreenSpacePos(4, ImVec2(900, 350));
    ImNodes::BeginNode(4);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Internet");
    ImNodes::EndNodeTitleBar();

    ImNodes::BeginInputAttribute(5);
    ImGui::Text("input");
    ImNodes::EndInputAttribute();

    ImNodes::EndNode();

    if (first_time)
        ImNodes::SetNodeScreenSpacePos(30, ImVec2(900, 600));
    ImNodes::BeginNode(30);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Tunnel 1 (trojan-go router)");
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(31);

    ImGui::Text("443");
    ImNodes::EndInputAttribute();

    ImNodes::BeginOutputAttribute(32);
    ImGui::Indent(180);
    ImGui::Text("output(NAT)");

    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();

    if (first_time)
        ImNodes::SetNodeScreenSpacePos(40, ImVec2(1400, 600));
    ImNodes::BeginNode(40);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Tunnel 2 (Ip-tables Forward)");
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(41);
    ImGui::Text("443");
    ImNodes::EndInputAttribute();
    ImNodes::BeginOutputAttribute(42);
    ImGui::Indent(180);
    ImGui::Text("output(NAT)");
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();



      if (first_time)
        ImNodes::SetNodeScreenSpacePos(50, ImVec2(300, 550));
    ImNodes::BeginNode(50);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Users");
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginOutputAttribute(51);
    ImGui::Indent(120);
    ImGui::Text("Connect");
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();


    ImNodes::Link(6, 3, 5);
    ImNodes::Link(200, 21, 20);
    ImNodes::Link(500, 51, 2);

    ImNodes::EndNodeEditor();
    first_time = false;
}