#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl.h"
#include <SDL.h>

#include "implot.h"
#include "imnodes.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imageloader.h"

#include "window.h"
#include "windows/leftsidebar_window.h"
#include "windows/main_window.h"
#include "windows/log_window.h"
#include "windows/login_window.h"
#include "views/simpleoverlay.h"
#include "keyboard.h"

extern void apply_theme_default();
extern void apply_theme_embraceTheDarkness();
extern SDL_Window *g_Window;
extern SDL_GLContext g_GLContext;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

ImGuiID dock_id_main;
ImGuiID dock_id_left;
ImGuiID dock_id_right;
ImGuiID dock_id_bottom;

bool AUTH = false;
bool FIRSTRUN = false;
bool BEGIN = false;

static bool show_app_metrics = false;

static bool ShowStyleSelector(const char *label)
{
    static int style_idx = 0;
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("x").x;

    ImGui::SetNextItemWidth(TEXT_BASE_WIDTH * 16);
    if (ImGui::Combo(label, &style_idx, "Default\0Light\0Classic\0Dark\0Darker"))
    {
        switch (style_idx)
        {
        case 0: // defailt
            ImGui::StyleColorsDark();
            apply_theme_default();
            ImPlot::StyleColorsAuto();
            ImNodes::StyleColorsDark();

            break;
        case 1: // light
            ImGui::StyleColorsLight();
            ImPlot::StyleColorsLight();
            ImNodes::StyleColorsLight();

            break;
        case 2: // classic
            ImGui::StyleColorsClassic();
            ImPlot::StyleColorsClassic();
            ImNodes::StyleColorsClassic();

            break;
        case 3: // dark
            ImGui::StyleColorsDark();
            ImPlot::StyleColorsDark();
            ImNodes::StyleColorsDark();

            break;
        case 4: // darker
            ImGui::StyleColorsDark();
            apply_theme_embraceTheDarkness();
            ImPlot::StyleColorsAuto();
            ImNodes::StyleColorsDark();

            break;
        }
        return true;
    }
    return false;
}

bool g_done = false;
int g_width;
int g_height;

void loop()
{

    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            g_done = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(g_Window))
            g_done = true;
    }
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    static bool open = true;

    // ImPlot::ShowDemoWindow(&open);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpaceRootWindow##MainFrame", &open, window_flags);
    ImGui::PopStyleVar(3);
    static bool DebugMode = true;

    static int my_image_width = 0;
    static int my_image_height = 0;
    static GLuint my_image_texture = 0;
    static bool ret = LoadTextureFromFile("resources/ic2.png", &my_image_texture, &my_image_width, &my_image_height);
    IM_ASSERT(ret);

    // ImGui::Begin("OpenGL Texture Text");
    // ImGui::Text("pointer = %p", my_image_texture);
    // ImGui::Text("size = %d x %d", my_image_width, my_image_height);
    // ImGui::Image((void *)(intptr_t)my_image_texture, ImVec2(my_image_width, my_image_height));
    // ImGui::End();

    // Menu Bar
    if (ImGui::BeginMenuBar())
    {

        // ImGui::Image((void *)(intptr_t)my_image_texture, ImVec2(30, 30));
        ImVec2 p = ImGui::GetCursorScreenPos();

        ImageRotated((void *)(intptr_t)my_image_texture, ImVec2(p.x + 15, p.y + 15), ImVec2(60, 60), 60);
        ImGui::SetCursorPosX(p.x + 60);
        if (ImGui::BeginMenu("Menu"))
        {

            ImGui::Text("Theme:");
            ImGui::SameLine();
            ShowStyleSelector("##Colors-Selector");
            if (ImGui::Selectable("check github for updates"))
            {
                EM_ASM(window.open("https://github.com/Leiren/Npanel", '_blank').focus(););
            }
            ImGui::Separator();
            if (ImGui::Selectable("logout"))
            {
                EM_ASM(
                    setCookie("token", "", 3);

                    window.location.reload(););
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {

            ImGui::MenuItem("Metrics/Debugger", NULL, &show_app_metrics, true);

            // ImGui::MenuItem("Metrics/Debugger", NULL, &show_app_metrics);
            // ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
            // ImGui::MenuItem("About", NULL, &show_app_about);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    const char *const DockSpaceRootNodeID = "DockSpaceRootNode##MainFrame";
    ImGuiID dockspace_root_id = ImGui::GetID(DockSpaceRootNodeID);

    if (ImGui::DockBuilderGetNode(dockspace_root_id) == NULL)
    {
        ImGui::DockBuilderRemoveNode(dockspace_root_id);                                        // Clear out existing layout
        ImGuiID nodeid = ImGui::DockBuilderAddNode(dockspace_root_id, ImGuiDockNodeFlags_None); // Add empty node

        ImGui::DockBuilderSetNodeSize(nodeid, viewport->Size);

        Window::dock_main_id = dockspace_root_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
        Window::dock_id_left = ImGui::DockBuilderSplitNode(Window::dock_main_id, ImGuiDir_Left, 0.10f, NULL, &Window::dock_main_id);
        Window::dock_id_right = ImGui::DockBuilderSplitNode(Window::dock_main_id, ImGuiDir_Right, 0.20f, NULL, &Window::dock_main_id);
        Window::dock_id_bottom = ImGui::DockBuilderSplitNode(Window::dock_main_id, ImGuiDir_Down, 0.25f, NULL, &Window::dock_main_id);
        ImGui::DockBuilderFinish(dockspace_root_id);
    }

    ImGui::DockSpace(dockspace_root_id, ImVec2(0.0f, 0.0f), 0);
    ImGui::End();

    static LeftSideBarWindow lsb_w;
    static MainWindow main_w;
    static LogWindow log_w;
    static LoginWindow login_w;
    OSK::thisframe = true;


    if (BEGIN)
    {
        if (AUTH && !FIRSTRUN)
        {
            lsb_w.tick();
            main_w.tick();
            log_w.tick();
        }
        else
            login_w.tick();
    }

    if (show_app_metrics)
        ImGui::ShowMetricsWindow(&show_app_metrics);

    // ImGui::ShowDemoWindow();
    static bool fframe = true;

    OSK::show();
    // drawOverlay();
    ///////////////////////////////
    ImGuiIO &io = ImGui::GetIO();

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(g_Window);
}
