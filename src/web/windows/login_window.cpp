#include "login_window.h"
#include "imgui.h"
#include "connection.h"
#include "customwidgets.h"
static void samelinehelpmarker(const char *desc)
{
    ImGui::SameLine();
    HelpMarker(desc);
    ImGui::SameLine();
}
using std::string;

extern bool AUTH;
extern bool FIRSTRUN;
static char input_password[80];
static bool has_error = false;
static string error = "";

static bool loading = false;
static bool req_succeed = false;

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
        ImGui::InputText("##login_password", input_password, IM_ARRAYSIZE(input_password), ImGuiInputTextFlags_Password);

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

        if (ImGui::Button("Clear", ImVec2(120, 0)))
        {
            input_password[0] = '\0';
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

static void state_begin()
{
    has_error = false;
    loading = false;
    req_succeed = false;
}
static void wizard_window()
{
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("x").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    static char input_domain[80];
    const char *items[] = {"Social Media1", "creative studio", "Pizza seler", "Direct TV", "Covido"};
    static char panel_final_url[120] = "url for Npanel:";

    static int old_state = 0;
    static int state = 0;
    if (state != old_state)
    {
        old_state = state;
        state_begin();
    }

    switch (state)
    {
    case 0:

        ImGui::Begin("Before we begin", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
        ImGui::Text("Thank you for using Npanel-UI.\n\n"
                    "   This UI is free and open source.\n"
                    "But, You have to take permission from the Author in order to create more than 30 users! why?\n"
                    "Because Backend Implementation is not part of Npanel-UI project.\n"
                    "You can build the UI from source code and Implement the server functions yourself \nto get rid of probably paying"
                    " a very small amount to help the author\nfor more features and improvements of this panel :(\n\n ");
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
        ImGui::Begin("Notes##notes1", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
        ImGui::Text("1- When you visit a url without https:// the browser will not allow you to use clipboard (CTRL + C or V)\n"
                    "so don't use it untill you finish setup.\n\n"
                    "2- Npanel always encrypts all packets with AES-256 regardless of the url protocol.\n\n"
                    "3-Npanel always kills ports 80,443 (for global serving) and ports 2060,2061 for local listening at startup.\n"
                    "to avoid problems.\n\n"
#ifndef win_build

                    "4-Npanel always disables ipv6 , enables bbr , disables ufw to avoid problems.\n\n"
#endif
                    "5-Npanel uses trojan-go for trojan protocol implementation, because indeed the author's experience confirms that it is the best way\n"
                    "to bypass GFW.\n\n"
                    "6-If you want to reduce the risk of your server getting blocked\ntell your users to use clients that support uTls. (lastest version of most clients do it)\n\n"
                    "7-Npanel is under active development. you are welcome to report any bugs you found and i will fix that asap!\n\n");
        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240 - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("Back", ImVec2(120, 0)))
            state--;
        ImGui::SameLine();
        if (ImGui::Button("Understood", ImVec2(120, 0)))
        {
            state++;
        }
        ImGui::End();
        break;
    case 2:
        ImGui::Begin("Domain", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

        ImGui::Text("domain:");
        samelinehelpmarker("The Domain you want to use for trojan.\n"
                           "example: sub.mydomain.com or mydomain.com ");
        ImGui::InputText("##input_domain", input_domain, IM_ARRAYSIZE(input_domain));
        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine();
        if (loading)
            Spinner("loading", 8, 2, IM_COL32(66, 150, 250, 255));
        if (has_error)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, Color::red);
            ImGui::TextUnformatted(error.c_str());
            ImGui::PopStyleColor();
        }
        if (has_error)
            ImGui::NewLine();
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240 - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("Back", ImVec2(120, 0)))
            state--;
        ImGui::SameLine();
        if (ImGui::Button("Next", ImVec2(120, 0)))
        {
            loading = true;
            Connection::send("wizard-1-domain", 1, input_domain)->connect([](Result res)
                                                                          {
                loading = false;
                if(res.success)
                    state++;
                else{
                    has_error = true;
                    error = res.info;
                } });
        }
        ImGui::End();
        break;

    case 3:
        ImGui::Begin("Certificate", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
        static char input_cert_path[80];
        static char input_private_key_path[80];
        ImGui::Text("certificate:");
        samelinehelpmarker("Certificate file path\n"
#ifndef win_build
                           "Start your path with /  and as you know it's full path!\n"
                           "This certificate must match the domain you enetered.\n"
                           "Examples: /etc/letsencrypt/live/my.domain.com/fullchain.pem\n"
                           "Examples: /etc/letsencrypt/live/my.domain.com/cert.pem"
#else
                           "Examples: C:\\ssl\\fullchain.pem \n"
                           "Examples: C:\\ssl\\cert.pem"
#endif

        );
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + TEXT_BASE_WIDTH);
        ImGui::InputText("##input_cert", input_cert_path, IM_ARRAYSIZE(input_cert_path));

        ImGui::Text("private key:");
        samelinehelpmarker("Private key file path\n"
#ifndef win_build
                           "Start your path with /  and as you know it's full path!\n"
                           "This key must match the domain you enetered.\n"
                           "Examples: /etc/letsencrypt/live/my.domain.com/privkey.pem"
#else
                           "Examples: C:\\ssl\\key.pem"
#endif

        );
        ImGui::InputText("##input_pkey", input_private_key_path, IM_ARRAYSIZE(input_private_key_path));
        ImGui::NewLine();
        if (ImGui::Button("How to get those?"))
        {
            EM_ASM(window.open("https://linuxiac.com/lets-encrypt-free-ssl-certificate/", '_blank').focus(););
        }
        samelinehelpmarker("Stop the Npanel to free port 80 before issuing a Let's Encrypt free SSL certificate.\n"
                           "\"service npanel stop\"");
        ImGui::NewLine();

        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine();
        if (loading)
            Spinner("loading", 8, 2, IM_COL32(66, 150, 250, 255));
        if (has_error)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, Color::red);
            ImGui::TextUnformatted(error.c_str());
            ImGui::PopStyleColor();
        }
        if (has_error)
            ImGui::NewLine();

        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240 - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("Back", ImVec2(120, 0)))
            state--;
        ImGui::SameLine();
        if (ImGui::Button("Next", ImVec2(120, 0)))
        {
            loading = true;
            Connection::send("wizard-2-cp", 2, input_cert_path, input_private_key_path)->connect([](Result res)
                                                                                                 {
                loading = false;
                if(res.success)
                    state++;
                else{
                    has_error = true;
                    error = res.info;
                } });
        }
        ImGui::End();
        break;
    case 4:
        ImGui::Begin("Notes##notes2", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
        ImGui::Text("All certificates will expire after a period of time (3 months forexample) \n\n"
                    "  It is your responsibility to renew it,  Let's Encrypt also sends you notify email before it expires.\n\n");
        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240 - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("Back", ImVec2(120, 0)))
            state--;
        ImGui::SameLine();
        if (ImGui::Button("Understood", ImVec2(120, 0)))
        {
            state++;
        }
        ImGui::End();
        break;

    case 5:
        static int input_fake_website_index = 0;
        ImGui::Begin("FakeWebsite", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
        ImGui::Text("Trojan-go (by default) will host all connections on port 443\n"
                    "and it is required to host a kinda (real) looking website on that port as well, so when a stranger\n"
                    "or a government worker opens the url, the fake website will be shown.\n\n"
                    "you can also change the port 443 to other ports later. but that is highly discouraged!\n\n");
        ImGui::Text("fakewebsite template:");
        samelinehelpmarker("Note: you are recommended to customize the html/css , files are in\n /opt/Npanel/templates\n");
        ImGui::Combo("##input_fake_website_index", &input_fake_website_index, items, IM_ARRAYSIZE(items));
        ImGui::NewLine();

        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine();
        if (loading)
            Spinner("loading", 8, 2, IM_COL32(66, 150, 250, 255));
        if (has_error)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, Color::red);
            ImGui::TextUnformatted(error.c_str());
            ImGui::PopStyleColor();
        }
        if (has_error)
            ImGui::NewLine();
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240 - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("Back", ImVec2(120, 0)))
            state--;
        ImGui::SameLine();
        if (ImGui::Button("Next", ImVec2(120, 0)))
        {
            loading = true;
            char buf[10];
            sprintf(buf, "%d", input_fake_website_index);
            Connection::send("wizard-3-fi", 1, buf)->connect([](Result res)
                                                             {
                loading = false;
                if(res.success)
                    state++;
                else{
                    has_error = true;
                    error = res.info;
                } });
        }
        ImGui::End();
        break;

    case 6:
        static char next_url_buf[120];

        ImGui::Begin("Admin Login information", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
        static char input_admin_username[80];
        static char input_admin_password[80];
        ImGui::Text("Read Hints!");
        ImGui::NewLine();
        ImGui::Text("Admin Username:");
        samelinehelpmarker("After passing this stage, when ever you want to login to the npanel\n"
                           "enter this url:  https://yourdomain/admin_username/\n\n"
                           "so it is clear that username is important same as password!");
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + TEXT_BASE_WIDTH);
        if (ImGui::InputText("##input_au", input_admin_username, IM_ARRAYSIZE(input_admin_username)))
        {
            sprintf(panel_final_url, "https://%s/%s/", input_domain, input_admin_username);
        }

        ImGui::Text("Admin Password:");
        samelinehelpmarker("After entering the correct url, you need the password to log-in\n");
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 2 * TEXT_BASE_WIDTH - 2);

        ImGui::InputText("##input_ap", input_admin_password, IM_ARRAYSIZE(input_admin_password));
        ImGui::NewLine();
        ImGui::Text("url for Npanel: %s %s", panel_final_url, "\nNote: the last slash in url is important!");

        ImGui::NewLine();

        ImGui::Separator();
        ImGui::NewLine();
        ImGui::SameLine();
        if (loading)
            Spinner("loading", 8, 2, IM_COL32(66, 150, 250, 255));
        if (has_error)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, Color::red);
            ImGui::TextUnformatted(error.c_str());
            ImGui::PopStyleColor();
        }
        if (has_error)
            ImGui::NewLine();

        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 240 - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("Back", ImVec2(120, 0)))
            state--;
        ImGui::SameLine();
        if (ImGui::Button("Finish", ImVec2(120, 0)))
        {
            loading = true;
            Connection::send("wizard-4-aup", 2, input_admin_username, input_admin_password)->connect([](Result res)
                                                                                                     {
                loading = false;
                if(res.success)
                    state++;
                else{
                    has_error = true;
                    error = res.info;
                } });
        }
        ImGui::End();

        break;
    case 7:
        ImGui::Begin("Complete", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
        ImGui::Text("The panel is restarting; if you made a mistake, the panel may not start, heres some debugging tips:\n\n"
#ifndef win_build

                    "1- check panel logs \"service npanel status\"\n"
                    "2- correct your mistakes in /opt/Npanel/panel.json\n"
                    "3- restart \"service npanel restart\"\n"
#else
                    "1- check panel logs from console window\n"
                    "2- correct your mistakes in Npanel/panel.json\n"
                    "3- restart the program"
#endif
        );
        if (ImGui::Button("Open Npanel"))
        {
            EM_ASM({window.location.href = UTF8ToString($0)}, panel_final_url);
        }
        ImGui::End();

        break;
    default:
        break;
    }
}