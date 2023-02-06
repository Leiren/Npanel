#include "implot.h"
#include "implot_internal.h"
#include "server_report_store.h"
#include <cmath>
#include "log.h"
#include "colors.h"
constexpr int chart_with = 30;
void server_status_frame()
{
    // ImPlot::ShowDemoWindow();
    static double x_data[chart_with] = {0};
    static double cpu_y_data[chart_with] = {0};
    static double memory_used_y_data[chart_with] = {0};

    static double users_upload_y_data[chart_with] = {0};
    static double users_download_y_data[chart_with] = {0};

    static bool is_first_time = true;
    static double memory_total = 0;

    static double server_total_upload;
    static double server_total_download;

    int cores_count = ServerReportStore::last_report.cores;
    int uptime_hrs = ServerReportStore::last_report.uptime / 60;
    int uptime_mins = ServerReportStore::last_report.uptime % 60;
    int server_capacity = ServerReportStore::last_report.server_capacity;
    int users_count = ServerReportStore::last_report.users_count;
    int active_users_count = ServerReportStore::last_report.active_users_count;

    if (is_first_time)
    {
        for (size_t i = 0; i < chart_with; i++)
        {
            x_data[i] = i;
        }

        is_first_time = false;
        ServerReportStore::signal.connect(
            [](ServerReport *report)
            {
                server_total_upload = 0;
                server_total_download = 0;
                size_t i = 0;
                for (; i < chart_with - 1; i++)
                {
                    cpu_y_data[i] = cpu_y_data[i + 1];
                    memory_used_y_data[i] = memory_used_y_data[i + 1];
                }
                cpu_y_data[i] = report->cpu_percent;
                memory_used_y_data[i] = report->memory_used / (1024.0); // to gb
                memory_total = round(report->memory_total / (1024.0));  // to gb

                int sum_d = 0;
                int sum_u = 0;
                for (int ui = 0; ui < report->users_count; ui++)
                {
                    sum_d += report->users[ui].speed_current.download;
                    sum_u += report->users[ui].speed_current.upload;
                    server_total_upload += report->users[ui].traffic_total.upload;
                    server_total_download += report->users[ui].traffic_total.download;
                }
                i = 0;
                for (; i < chart_with - 1; i++)
                {
                    users_upload_y_data[i] = users_upload_y_data[i + 1];
                    users_download_y_data[i] = users_download_y_data[i + 1];
                }
                users_upload_y_data[i] = sum_u / (1024.0);
                users_download_y_data[i] = sum_d / (1024.0); 
                server_total_upload = server_total_upload / (1024.0 * 1024.0);//kb to gb
                server_total_download = server_total_download / (1024.0 * 1024.0);//kb to gb
            });
    }

    ImVec2 content_avail = ImGui::GetContentRegionAvail();
    const float space = ImGui::GetStyle().ItemSpacing.x / 2;

    const float cpu_plot_w = std::max(content_avail.x / 2.0f - space, 4.0f);
    if (ImPlot::BeginPlot("Cpu ###eoauaoeu", ImVec2(cpu_plot_w, 0), ImPlotFlags_NoMenus))
    {

        ImPlot::SetupAxes("", "", ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight, ImPlotAxisFlags_NoHighlight);

        ImPlot::SetupAxesLimits(0, chart_with - 1, 0, 100, ImGuiCond_Always);

        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::PlotShaded("Cpu used %", x_data, cpu_y_data, chart_with, -INFINITY);
        ImPlot::PopStyleVar();
        ImPlot::PlotLine("Cpu used %", x_data, cpu_y_data, chart_with);

        ImPlot::EndPlot();
    }
    ImGui::SameLine();

    if (ImPlot::BeginPlot("Memory", ImVec2(-1, 0), ImPlotFlags_NoMenus))
    {
        // ImPlot::SetupAxes("", "",ImPlotAxisFlags_Lock | ImPlotAxisFlags_RangeFit ,ImPlotAxisFlags_Lock |  ImPlotAxisFlags_RangeFit );
        ImPlot::SetupAxes("", "", ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight, ImPlotAxisFlags_NoHighlight);

        ImPlot::SetupAxesLimits(0, chart_with - 1, 0, memory_total, ImGuiCond_Always);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::PlotShaded("Memory used GBytes", x_data, memory_used_y_data, chart_with, -INFINITY);
        ImPlot::PopStyleVar();
        ImPlot::PlotLine("Memory used GBytes", x_data, memory_used_y_data, chart_with);

        ImPlot::EndPlot();
    }
    const int child_w = cpu_plot_w;
    ImGui::BeginChild("ChildR", ImVec2(child_w, 300), true, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar())
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (child_w / 2 - ImGui::CalcTextSize("Server Information").x / 2));
        ImGui::Text("Server Information");

        ImGui::EndMenuBar();
        // ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 250);
    }
    if (ImGui::BeginTable("split", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings))
    {

        ImGui::TableNextColumn();
        ImGui::Button("UpTime", ImVec2(-FLT_MIN, 0.0f));
        ImGui::TableNextColumn();
        static char buf[30];
        sprintf(buf, "%d hrs %d m", uptime_hrs, uptime_mins);
        ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));

        ImGui::TableNextColumn();
        ImGui::Button("Cpu Cores", ImVec2(-FLT_MIN, 0.0f));
        ImGui::TableNextColumn();
        sprintf(buf, "%d", cores_count);
        ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));

        ImGui::TableNextColumn();
        ImGui::Button("Memory", ImVec2(-FLT_MIN, 0.0f));
        ImGui::TableNextColumn();
        sprintf(buf, "%.01lf GB", memory_total);
        ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));

        ImGui::TableNextColumn();
        ImGui::Button("Users Count", ImVec2(-FLT_MIN, 0.0f));
        ImGui::TableNextColumn();
        sprintf(buf, "%d", users_count);
        ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));

        ImGui::TableNextColumn();
        ImGui::Button("Online Users", ImVec2(-FLT_MIN, 0.0f));
        ImGui::TableNextColumn();
        sprintf(buf, "%d", active_users_count);
        ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));

        ImGui::TableNextColumn();
        ImGui::Button("Total Server Upload", ImVec2(-FLT_MIN, 0.0f));
        ImGui::TableNextColumn();
        sprintf(buf, "%.2f GB", server_total_upload);
        ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));

        ImGui::TableNextColumn();
        ImGui::Button("Total Server Download", ImVec2(-FLT_MIN, 0.0f));
        ImGui::TableNextColumn();
        sprintf(buf, "%.2f GB", server_total_download);
        ImGui::Button(buf, ImVec2(-FLT_MIN, 0.0f));

        ImGui::TableNextColumn();
        ImGui::Button("Server Capacity Grade", ImVec2(-FLT_MIN, 0.0f));
        ImGui::TableNextColumn();
        const char *server_grade;
        switch (server_capacity)
        {
        case 3:
            ImGui::PushStyleColor(ImGuiCol_Button, Color::green);
            server_grade = "A+";

            break;
        case 2:
            ImGui::PushStyleColor(ImGuiCol_Button, Color::green);
            server_grade = "A";

            break;
        case 1:
            ImGui::PushStyleColor(ImGuiCol_Button, Color::orange);
            server_grade = "B";

            break;
        case 0:
            ImGui::PushStyleColor(ImGuiCol_Button, Color::orange);
            server_grade = "C";

            break;

        default:
            ImGui::PushStyleColor(ImGuiCol_Button, Color::transparent);
            server_grade = "undetermined";
            break;
        }
        ImGui::Button(server_grade, ImVec2(-FLT_MIN, 0.0f));
        ImGui::PopStyleColor();

        // ImGui::TableNextColumn();
        // ImGui::Button("Software Last-Update", ImVec2(-FLT_MIN, 0.0f));
        // ImGui::TableNextColumn();
        // ImGui::Button(last_software_update, ImVec2(-FLT_MIN, 0.0f));

        ImGui::TableNextColumn();
        ImGui::Button("Software Version", ImVec2(-FLT_MIN, 0.0f));
        ImGui::TableNextColumn();
        // ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.246f, 0.836f, 0.051f, 0.65f));
        ImGui::Button("0.0.7 Alpha ", ImVec2(-FLT_MIN, 0.0f));
        // ImGui::PopStyleColor();

        // ImGui::TableNextColumn();
        // ImGui::Button("Software Release Date", ImVec2(-FLT_MIN, 0.0f));
        // ImGui::TableNextColumn();
        // ImGui::Button("Thu, 26 Jan 2023 19:31:16", ImVec2(-FLT_MIN, 0.0f));

        ImGui::EndTable();
    }
    ImGui::EndChild();

    ImGui::SameLine();

    content_avail = ImGui::GetContentRegionAvail();
    if (ImPlot::BeginPlot("Users Traffic", ImVec2(-1, 0), ImPlotFlags_NoMenus))
    {
        // ImPlot::SetupAxes("", "",ImPlotAxisFlags_Lock | ImPlotAxisFlags_RangeFit ,ImPlotAxisFlags_Lock |  ImPlotAxisFlags_RangeFit );
        ImPlot::SetupAxes("", "", ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoMenus | ImPlotAxisFlags_NoHighlight, ImPlotAxisFlags_NoHighlight);

        ImPlot::SetupAxesLimits(0, chart_with - 1, 0, 125, ImGuiCond_Always);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::PlotShaded("Upload MByte/s", x_data, users_upload_y_data, chart_with, -INFINITY);
        ImPlot::PlotShaded("Download MByte/s", x_data, users_download_y_data, chart_with, -INFINITY);
        ImPlot::PopStyleVar();

        ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::GetColormapColor(2));
        ImPlot::PlotLine("Upload MByte/s", x_data, users_upload_y_data, chart_with);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::GetColormapColor(3));

        ImPlot::PlotLine("Download MByte/s", x_data, users_download_y_data, chart_with);
        ImPlot::PopStyleColor();

        ImPlot::EndPlot();
    }
}