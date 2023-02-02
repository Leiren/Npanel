#include "imgui.h"
#include "imgui_internal.h"
#include "server_report_store.h"
#include <cmath>
#include "log.h"
#include "colors.h"
#include "connection.h"
extern void HelpMarker(const char *desc);

void ToggleButton(const char *str_id, bool *v);
void user_opt_popup(User *);
extern void new_user_popup_frame(bool *new_state);
extern void edit_user_popupframe(User **_user);
extern void delete_user_popupframe(User **_user);
extern void view_notes_popupframe(User **_user);
extern void show_user_configs(User **_user);
extern void error_popupframe();
static User *share_user = nullptr;
static User *edit_user = nullptr;
static User *delete_user = nullptr;
static User *notes_user = nullptr;
bool live_data = true;
static ImGuiTextFilter Filter;
enum UserRowModelColumnID
{
    UserRowModelColumnID_ID,
    UserRowModelColumnID_Name,
    UserRowModelColumnID_Action_Enable,
    UserRowModelColumnID_Action_Options,
    UserRowModelColumnID_Upload_Speed,
    UserRowModelColumnID_Download_Speed,
    UserRowModelColumnID_Total_Download,
    UserRowModelColumnID_Total_Upload,
    UserRowModelColumnID_days_left
    // UserRowModelColumnID_Upload_Speed_Limit,
    // UserRowModelColumnID_Download_Speed_Limit,
};

struct UserRowModel
{

    // We have a problem which is affecting _only this demo_ and should not affect your code:
    // As we don't rely on std:: or other third-party library to compile dear imgui, we only have reliable access to qsort(),
    // however qsort doesn't allow passing user data to comparing function.
    // As a workaround, we are storing the sort specs in a static/global for the comparing function to access.
    // In your own use case you would probably pass the sort specs to your sorting/comparing functions directly and not use a global.
    // We could technically call ImGui::TableGetSortSpecs() in CompareWithSortSpecs(), but considering that this function is called
    // very often by the sorting algorithm it would be a little wasteful.
    static const ImGuiTableSortSpecs *s_current_sort_specs;

    // Compare function to be used by qsort()
    static int CompareWithSortSpecs(const void *lhs, const void *rhs)
    {
        const User *a = ((const User **)lhs)[0];
        const User *b = ((const User **)rhs)[0];
        for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
        {
            // Here we identify columns using the ColumnUserID value that we ourselves passed to TableSetupColumn()
            // We could also choose to identify columns based on their index (sort_spec->ColumnIndex), which is simpler!
            const ImGuiTableColumnSortSpecs *sort_spec = &s_current_sort_specs->Specs[n];
            int delta = 0;
            switch (sort_spec->ColumnUserID)
            {
            case UserRowModelColumnID_ID:
                delta = (a->id - b->id);
                break;
            case UserRowModelColumnID_Name:
                delta = (strcmp(a->name.c_str(), b->name.c_str()));
                break;

            case UserRowModelColumnID_Upload_Speed:
                delta = (a->speed_current.upload - b->speed_current.upload);
                break;
            case UserRowModelColumnID_Download_Speed:
                delta = (a->speed_current.download - b->speed_current.download);
                break;
            case UserRowModelColumnID_Total_Download:
                delta = (a->traffic_total.download - b->traffic_total.download);
                break;
            case UserRowModelColumnID_Total_Upload:
                delta = (a->traffic_total.download - b->traffic_total.download);
                break;
            case UserRowModelColumnID_days_left:
                delta = (a->days_left - b->days_left);
                break;

            default:
                IM_ASSERT(0);
                break;
            }
            if (delta > 0)
                return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
            if (delta < 0)
                return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
        }

        // qsort() is instable so always return a way to differenciate items.
        // Your own compare function may want to avoid fallback on implicit sort specs e.g. a Name compare if it wasn't already part of the sort specs.
        return (a->id - b->id);
    }
};
const ImGuiTableSortSpecs *UserRowModel::s_current_sort_specs = NULL;

static void PushStyleCompact()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
}

static void PopStyleCompact()
{
    ImGui::PopStyleVar(2);
}

void render_list()
{
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

    // static ImVector<UserRowModel> items;
    User *real_users = ServerReportStore::last_report.users;
    static int users_size = 0;
    static User **users = (User **)malloc(users_size * sizeof(User *));
    if (users_size != ServerReportStore::last_report.users_count)
    {
        users_size = ServerReportStore::last_report.users_count;
        users = (User **)realloc(users, users_size * sizeof(User *));

        for (size_t i = 0; i < users_size; i++)
        {
            users[i] = &real_users[i];
        }
    }

    // Options
    static ImGuiTableFlags flags =
        ImGuiTableFlags_SortTristate | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_ScrollY;
    // PushStyleCompact();
    // ImGui::CheckboxFlags("ImGuiTableFlags_SortMulti", &flags, ImGuiTableFlags_SortMulti);
    // ImGui::SameLine();
    // ImGui::CheckboxFlags("ImGuiTableFlags_SortTristate", &flags, ImGuiTableFlags_SortTristate);
    // PopStyleCompact();
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

    if (ImGui::BeginTable("table_sorting", 9, flags, ImVec2(0.0f, 0.0f), 0.0f))
    {
        // Declare columns
        // We use the "user_id" parameter of TableSetupColumn() to specify a user id that will be stored in the sort specifications.
        // This is so our sort function can identify a column given our own identifier. We could also identify them based on their index!
        // Demonstrate using a mixture of flags among available sort-related flags:
        // - ImGuiTableColumnFlags_DefaultSort
        // - ImGuiTableColumnFlgas_NoSort / ImGuiTableColumnFlags_NoSortAscending / ImGuiTableColumnFlags_NoSortDescending
        // - ImGuiTableColumnFlags_PreferSortAscending / ImGuiTableColumnFlags_PreferSortDescending
        ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 3, UserRowModelColumnID_ID);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 16, UserRowModelColumnID_Name);
        ImGui::TableSetupColumn("Enable", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, UserRowModelColumnID_Action_Enable);
        ImGui::TableSetupColumn("Options##top_list", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 10, UserRowModelColumnID_Action_Options);

        ImGui::TableSetupColumn("Current Upload Speed", ImGuiTableColumnFlags_WidthFixed, 0.0f, UserRowModelColumnID_Upload_Speed);
        ImGui::TableSetupColumn("Current Download Speed", ImGuiTableColumnFlags_WidthFixed, 0.0f, UserRowModelColumnID_Download_Speed);
        ImGui::TableSetupColumn("Total Upload", ImGuiTableColumnFlags_WidthFixed, 0.0f, UserRowModelColumnID_Total_Download);
        ImGui::TableSetupColumn("Total Download", ImGuiTableColumnFlags_WidthFixed, 0.0f, UserRowModelColumnID_Total_Upload);
        ImGui::TableSetupColumn("days left", ImGuiTableColumnFlags_WidthFixed, 0.0f, UserRowModelColumnID_days_left);

        ImGui::TableSetupScrollFreeze(0, 1); // Make row always visible
        ImGui::TableHeadersRow();

        // Sort our data if sort specs have been changed!
        if (ImGuiTableSortSpecs *sorts_specs = ImGui::TableGetSortSpecs())
            if (sorts_specs->SpecsDirty)
            {
                UserRowModel::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
                if (users_size > 1)
                    qsort(&users[0], (size_t)users_size, sizeof(users[0]), UserRowModel::CompareWithSortSpecs);
                UserRowModel::s_current_sort_specs = NULL;
                sorts_specs->SpecsDirty = false;
            }

        // Demonstrate using clipper for large vertical lists
        int size_filtered = users_size;
        static User *passed_indexs[500];
        if (Filter.IsActive())
        {
            size_filtered = 0;

            for (int row_n = 0; row_n < users_size; row_n++)
            {
                if (Filter.PassFilter(users[row_n]->name.c_str()))
                {
                    passed_indexs[size_filtered] = users[row_n];

                    size_filtered++;
                }
            }
        }

        ImGuiListClipper clipper;
        clipper.Begin(size_filtered);
        while (clipper.Step())
            for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
            {
                // Display a data item
                User *item = Filter.IsActive() ? passed_indexs[row_n] : users[row_n];

                ImGui::PushID(item->id);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                static char id_buf[50];
                int digits = sprintf(id_buf, "%d", item->id);

                ImGui::Indent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * digits / 2.0f - ImGui::GetStyle().FramePadding.x);
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted(id_buf);
                ImGui::Unindent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * digits / 2.0f - ImGui::GetStyle().FramePadding.x);
                ImGui::TableNextColumn();
                // ImGui::Indent(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(),1).GetWidth()/2 - TEXT_BASE_WIDTH*strlen(item->Name)/2 - ImGui::GetStyle().FramePadding.x);
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted(item->name.c_str());
                // ImGui::Unindent(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(),1).GetWidth()/2 - TEXT_BASE_WIDTH*strlen(item->Name)/2 - ImGui::GetStyle().FramePadding.x);

                ImGui::TableNextColumn();
                ImGui::Indent(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 2).GetWidth() / 2.0f - TEXT_BASE_WIDTH * 2.65f - ImGui::GetStyle().FramePadding.x);
                bool c_enable = item->enable;
                ToggleButton("Active", &item->enable);
                if (item->enable != c_enable)
                {
                    Connection::updateUser(*item);
                }
                ImGui::Unindent(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 2).GetWidth() / 2.0f - TEXT_BASE_WIDTH * 2.65f - ImGui::GetStyle().FramePadding.x);

                ImGui::TableNextColumn();
                if (ImGui::Button("Options", ImVec2(TEXT_BASE_WIDTH * 10, 0)))
                {
                    ImGui::OpenPopup("user_opt_popup");
                }
                user_opt_popup(item);

                char showbuf[25];

                ImGui::TableNextColumn();
                sprintf(showbuf, "%d KB/s", item->speed_current.upload);
                ImGui::Indent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 4).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * strlen(showbuf) / 2.0f - ImGui::GetStyle().FramePadding.x);
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted(showbuf);
                ImGui::Unindent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 4).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * strlen(showbuf) / 2.0f - ImGui::GetStyle().FramePadding.x);

                ImGui::TableNextColumn();
                sprintf(showbuf, "%d KB/s", item->speed_current.download);
                ImGui::Indent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 5).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * strlen(showbuf) / 2.0f - ImGui::GetStyle().FramePadding.x);
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted(showbuf);
                ImGui::Unindent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 5).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * strlen(showbuf) / 2.0f - ImGui::GetStyle().FramePadding.x);

                ImGui::TableNextColumn();
                sprintf(showbuf, "%d KB/s", item->traffic_total.upload);
                ImGui::Indent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 6).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * strlen(showbuf) / 2.0f - ImGui::GetStyle().FramePadding.x);
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted(showbuf);
                ImGui::Unindent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 6).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * strlen(showbuf) / 2.0f - ImGui::GetStyle().FramePadding.x);

                ImGui::TableNextColumn();
                ImGui::Indent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 7).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * strlen(showbuf) / 2.0f - ImGui::GetStyle().FramePadding.x);
                sprintf(showbuf, "%d KB/s", item->traffic_total.download);
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted(showbuf);
                ImGui::Unindent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 7).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * strlen(showbuf) / 2.0f - ImGui::GetStyle().FramePadding.x);

                ImGui::TableNextColumn();
                if (item->days_left == 0)
                    sprintf(showbuf, "no-limit");
                else
                    sprintf(showbuf, "%d", item->days_left);

                ImGui::Indent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 8).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * strlen(showbuf) / 2.0f - ImGui::GetStyle().FramePadding.x);
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted(showbuf);
                ImGui::Unindent((ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 8).GetWidth() - 8.0f) / 2.0f - TEXT_BASE_WIDTH * strlen(showbuf) / 2.0f - ImGui::GetStyle().FramePadding.x);

                // ImGui::SmallButton("None");
                // ImGui::TableNextColumn();
                // ImGui::Text("%d", item->Quantity);

                ImGui::PopID();
            }

        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}

void user_opt_popup(User *user)
{
    if (ImGui::BeginPopup("user_opt_popup"))
    {
        const char *names[] = {"Share", "Edit", "Notes", "Reset Traffics", "Delete"};

        // ImGui::Text("user options:");
        // ImGui::Separator();
        for (int i = 0; i < IM_ARRAYSIZE(names) - 1; i++)
        {
            if (ImGui::Selectable(names[i]))
            {
                switch (i)
                {
                case 0:
                    share_user = user;
                    break;
                case 1:
                    edit_user = user;
                    break;
                case 2:
                    notes_user = user;
                    break;
                case 3:
                    break;
                }
            }
            if (i == 3)
            {
                ImGui::SameLine();
                HelpMarker("This will reset Total Traffics used, this is useful when the user has reached the traffic limit.");
            }
        }
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, Color::red);
        if (ImGui::Selectable(names[IM_ARRAYSIZE(names) - 1]))
        {
            delete_user = user;
        }
        ImGui::PopStyleColor();

        ImGui::EndPopup();
    }
}

void users_view_frame()
{
    static bool new_state_for_nup = false;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

    ImGui::PushStyleColor(ImGuiCol_TableBorderLight, Color::softblack);

    if (ImGui::Button("Create User", ImVec2(240, 0)))
    {
        ImGui::OpenPopup("Enter Information##new_user_popup");
        new_state_for_nup = true;
    }
    ImGui::SameLine();

    if (ImGui::Button(live_data ? "Live Data" : "Paused", ImVec2(140, 0)))
    {
        live_data = !live_data;
    }
    ImGui::SameLine();

    // if (ImGui::Button("Order"))
    // {
    // }
    Filter.Draw("Search", 180);
    ImGui::Separator();
    // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 0));

    render_list();
    new_user_popup_frame(&new_state_for_nup);

    if (share_user != nullptr)
        ImGui::OpenPopup("Configs##share_user_popup");
    show_user_configs(&share_user);

    if (edit_user != nullptr)
        ImGui::OpenPopup("Enter Information##edit_user_popup");
    edit_user_popupframe(&edit_user);

    if (delete_user != nullptr)
        ImGui::OpenPopup("Confirm##delete_user_popup");
    delete_user_popupframe(&delete_user);

    if (notes_user != nullptr)
        ImGui::OpenPopup("Notes##show_notes_user_popup");
    view_notes_popupframe(&notes_user);

    error_popupframe();

    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor();
}

void ToggleButton(const char *str_id, bool *v)
{
    ImVec4 *colors = ImGui::GetStyle().Colors;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.75f;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked())
        *v = !*v;
    ImGuiContext &gg = *ImGui::GetCurrentContext();
    float ANIM_SPEED = 0.085f;
    if (gg.LastActiveId == gg.CurrentWindow->GetID(str_id)) // && g.LastActiveIdTimer < ANIM_SPEED)
        float t_anim = ImSaturate(gg.LastActiveIdTimer / ANIM_SPEED);
    if (*v)
        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_ButtonActive] : ImVec4(0.78f, 0.78f, 0.78f, 1.0f)), height * 0.5f);
    else
        draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), ImGui::GetColorU32(*v ? colors[ImGuiCol_Button] : ImVec4(0.85f, 0.85f, 0.85f, 1.0f)), height * 0.50f);
    draw_list->AddCircleFilled(ImVec2(p.x + radius + (*v ? 1 : 0) * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}