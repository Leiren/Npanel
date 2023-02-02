
#include "window.h"
#include "log.h"

ImGuiID Window::dock_main_id;
ImGuiID Window::dock_id_left;
ImGuiID Window::dock_id_right;
ImGuiID Window::dock_id_bottom;

Window::Window(WindowProps winargs) : window_properties(winargs),
                                      _visible(true),
                                      visible_ptr(&_visible),
                                      re_dock(false)
{
    // Exceptionally add an extra assert here for people confused about initial Dear ImGui setup
    // Most ImGui functions would normally just crash if the context is missing.
    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing dear imgui context. Refer to examples app!");
    console.log("[Application] create window: %s  ", winargs.Title);

    // LOG_TRACE("[Application] create window: {0:<20}  ", winargs.Title );
    // LOG_INFO("create new window: {0:<20} , Pos: (X: {1:<5d} , Y: {2:<5d}) Width: {3:<5d} Height: {4:<5d} ", winargs.Title, (int)winargs.Position.x, (int)winargs.Position.y, (int)winargs.Size.x, (int)winargs.Size.y);
}
