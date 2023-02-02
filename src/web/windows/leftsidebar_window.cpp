#include "leftsidebar_window.h"
rocket::signal<void(int)>  LeftSideBarWindow::on_tab_changed;

void DrawRowsBackground(int row_count, float line_height, float x1, float x2, float y_offset, ImU32 col_even, ImU32 col_odd)
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    float y0 = ImGui::GetCursorScreenPos().y + (float)(int)y_offset;

    int row_display_start;
    int row_display_end;
    ImGui::CalcListClipping(row_count, line_height, &row_display_start, &row_display_end);
    for (int row_n = row_display_start; row_n < row_display_end; row_n++)
    {
        ImU32 col = (row_n & 1) ? col_odd : col_even;
        if ((col & IM_COL32_A_MASK) == 0)
            continue;
        float y1 = y0 + (line_height * row_n);
        float y2 = y1 + line_height;
        draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col);
    }
}

void LeftSideBarWindow::onFrame()
{
    static const char *list[] = {
        "Server Status",
        "Users",
        "Panel Settings",
        "Bash",
        "Tunnels",
        "About"

    };
    int list_count = sizeof(list) / sizeof(list[0]);

    float x1 = ImGui::GetCurrentWindow()->WorkRect.Min.x;
    float x2 = ImGui::GetCurrentWindow()->WorkRect.Max.x;
    float item_spacing_y = ImGui::GetStyle().ItemSpacing.y;
    float item_offset_y = -item_spacing_y * 0.5f;
    float line_height = ImGui::GetTextLineHeight() + item_spacing_y;
    DrawRowsBackground(list_count, line_height, x1, x2, item_offset_y, 0, ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.4f, 0.15f)));


    ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered));
    static int selected_index = -1;
    for (int n = 0; n < list_count; n++)
    {
        bool x = selected_index == n;
        ImGui::Indent(ImGui::GetFontSize() / 2);

        if (ImGui::Selectable(list[n], &x, ImGuiSelectableFlags_AllowDoubleClick))
        {
            if (x)
            {
                selected_index = n;
                on_tab_changed(n);
                // console.log("change index: %d",n);
            }
        }
        ImGui::Unindent(ImGui::GetFontSize() / 2);
    }
    ImGui::Separator();
    //  ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)
    ImGui::PopStyleColor();
}