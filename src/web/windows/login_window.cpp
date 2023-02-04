#include "login_window.h"
#include "imgui.h"
#include "connection.h"

using std::string;

extern bool AUTH;
extern bool FIRSTRUN;
static char input_password[80];
static bool has_error = false;
static string error = "";

static void error_popupframe()
{
    if (has_error)
        ImGui::OpenPopup("Error##loginpage");
    ImGui::SetNextWindowSize(ImVec2(600, -1));
    bool meaningless = true;
    if (ImGui::BeginPopupModal("Error##loginpage", &meaningless, ImGuiWindowFlags_NoSavedSettings))
    {

        has_error = false;
        ImGui::TextWrapped("%s", error.c_str());

        ImGui::EndPopup();
    }
}
void LoginWindow::try_login()
{

    Connection::send("Auth", 1, input_password)->connect([](Result result)
                                                         {
                                                     if (result.success)
                                                     {
                                                         // loged in
                                                         Connection::token = result.info;
                                                         EM_ASM({
                                                            const str = UTF8ToString($0);
                                                            //  alert(str);
                                                             setCookie("token", str, 3);
                                                         },
                                                                Connection::token.c_str());

                                                         AUTH = true;
                                                     }
                                                     else
                                                     {
                                                         // pass wrong
                                                         error = "Server: Incorrect password.";
                                                         has_error = true;

                                                         AUTH = false;
                                                     } });
}
static void wizard_window();
void LoginWindow::onFrame()
{
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("x").x;

    if (!AUTH && !FIRSTRUN)
    {
        auto content_avail = ImGui::GetContentRegionAvail();
        ImGui::Dummy(ImVec2(0, content_avail.y * 30 / 100));
        const int x_pad = content_avail.x * 33 / 100;
        const int win_w = ImGui::GetWindowWidth();
        ImGui::SetCursorPosX(win_w / 2 - TEXT_BASE_WIDTH * 34 / 2 - ImGui::GetStyle().ItemSpacing.x / 2);
        ImGui::Text("Password:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 25);
        ImGui::InputText("##login_password", input_password, IM_ARRAYSIZE(input_password));

        ImGui::SetCursorPosX(x_pad);
        ImGui::SetNextItemWidth(x_pad);
        ImGui::Separator();

        ImGui::SetCursorPosX(win_w / 2 - 120 - ImGui::GetStyle().ItemSpacing.x / 2);

        if (ImGui::Button("OK", ImVec2(120, 0)) || (ImGui::IsKeyPressed(ImGuiKey_Enter)))
        {
            try_login();
            if (!has_error)
            {
                ImGui::CloseCurrentPopup();
            }
        }
        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        error_popupframe();
    }

    if (AUTH && FIRSTRUN)
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(ImGui::GetStyle().CellPadding.x, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 5));

        wizard_window();
        ImGui::PopStyleVar(2);
    }
}

static void wizard_window()
{
    static int state = 0;
    switch (state)
    {
    case 0:
        ImGui::Begin("Before we begin", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
        ImGui::Text("Thank you for using Npanel-UI.\n\n"
                    "   This UI is free and open source.\n"
                    "But, You have to take permission from the Author in order to create more than 50 users! why?\n"
                    "Because Backend Implementation is not part of Npanel-UI project.\n"
                    "You can build the UI from source code and Implement the server functions yourself \nto get rid of probably paying"
                    " a very! small amount to help the author\nfor more features and improvements of this panel :(\n\n ");
        ImGui::Text("You will find the Author contact information in panel about page.\n\n");
        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240 - ImGui::GetStyle().ItemSpacing.x);
        ImGui::Button("Reject", ImVec2(120, 0));
        ImGui::SameLine();
        if (ImGui::Button("Agree", ImVec2(120, 0)))
        {
            state++;
        }
        ImGui::End();
        break;

    case 1:
        ImGui::Begin("Domain", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);

        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240 - ImGui::GetStyle().ItemSpacing.x);
        ImGui::Button("Back", ImVec2(120, 0));
        ImGui::SameLine();
        if (ImGui::Button("Next", ImVec2(120, 0)))
        {
            state++;
        }
        ImGui::End();
        break;
    default:
        break;
    }
}