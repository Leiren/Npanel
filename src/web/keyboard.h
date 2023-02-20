
#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include "log.h"
#define SDL_h_
extern bool PHONE;

namespace OSK
{
    static inline ImVec2 operator-(const ImVec2 &lhs, const ImVec2 &rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
    static inline ImVec2 operator+(const ImVec2 &lhs, const ImVec2 &rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }

    static const char *GetInputSourceName(ImGuiInputSource source)
    {
        const char *input_source_names[] = {"None", "Mouse", "Keyboard", "Gamepad", "Nav", "Clipboard"};
        IM_ASSERT(IM_ARRAYSIZE(input_source_names) == ImGuiInputSource_COUNT && source >= 0 && source < ImGuiInputSource_COUNT);
        return input_source_names[source];
    }

    struct Key
    {
        const char *lib = nullptr;
        unsigned int order;
        unsigned int scanCodePage1 = 0; // win32 scancode
        unsigned int scanCodePage7 = 0; // HID (SDL,...)
        float offset = 0;
        float width = 40;
        bool hold = false;
    };

    static Key Keys[5][18] = {

        {{"~", 20, 0x29, 0x35}, {"1", 21, 0x2, 0x1E}, {"2", 22, 0x3, 0x1F}, {"3", 23, 0x4, 0x20}, {"4", 24, 0x5, 0x21}, {"5", 25, 0x6, 0x22}, {"6", 26, 0x7, 0x23}, {"7", 27, 0x8, 0x24}, {"8", 28, 0x9, 0x25}, {"9", 29, 0xA, 0x26}, {"0", 30, 0xB, 0x27}, {"-", 31, 0xC, 0x2D}, {"+", 32, 0xD, 0x2E}, {"<", 33, 0xE, 0x2A, 0, 80}},
        {{"Tab", 3, 0xF, 0x2B, 0, 60}, {"Q", 37, 0x10, 0x14}, {"W", 38, 0x11, 0x1A}, {"E", 39, 0x12, 0x08}, {"R", 40, 0x13, 0x15}, {"T", 41, 0x14, 0x17}, {"Y", 42, 0x15, 0x1C}, {"U", 43, 0x16, 0x18}, {"I", 44, 0x17, 0x0C}, {"O", 45, 0x18, 0x12}, {"P", 46, 0x19, 0x13}, {"[", 47, 0x1A, 0x2F}, {"]", 48, 0x1B, 0x30}, {"|", 49, 0x2B, 0x31, 0, 60}},
        {{"Caps Lock", 53, 0x3A, 0x39, 0, 80}, {"A", 54, 0x1E, 0x04}, {"S", 55, 0x1F, 0x16}, {"D", 56, 0x20, 0x07}, {"F", 57, 0x21, 0x09}, {"G", 58, 0x22, 0x0A}, {"H", 59, 0x23, 0x0B}, {"J", 60, 0x24, 0x0D}, {"K", 61, 0x25, 0x0E}, {"L", 62, 0x26, 0x0F}, {";", 63, 0x27, 0x33}, {"'", 64, 0x28, 0x34}, {"Ret", 65, 0x1C, 0X28, 0, 84}},
        {
            {"Shift", 2, 0x2A, 0xE1, 0, 104},
            {"Z", 66, 0x2C, 0x1D},
            {"X", 67, 0x2D, 0x1B},
            {"C", 68, 0x2E, 0x06},
            {"V", 69, 0x2F, 0x19},
            {"B", 70, 0x30, 0x05},
            {"N", 71, 0x31, 0x11},
            {"M", 72, 0x32, 0x10},
            {",", 73, 0x33, 0x36},
            {".", 74, 0x34, 0x37},
            {"/", 75, 0x35, 0x38},
            {"Shift", 2, 0x2A, 0xE5, 0, 104},
        },
        {{"Ctrl", 0, 0x1D, 0xE0, 0, 60}, {"Alt", 1, 0x38, 0xE2, 68, 60}, {"Space", 77, 0x39, 0X2c, 0, 260}, {"Alt", 1, 0x38, 0xE6, 0, 60}, {"Ctrl", 0, 0x1D, 0xE4, 68, 60}}};

    //     static const Key& GetKeyForScanCode(unsigned int scancode)
    //     {
    //         for (unsigned int y = 0; y < 6; y++)
    //         {
    //             int x = 0;
    //             while (Keys[y][x].lib)
    //             {
    // #ifdef SDL_h_
    //                 if (Keys[y][x].scanCodePage7 == scancode)
    // #elif WIN32
    //                 if (Keys[y][x].scanCodePage1 == scancode)
    // #else
    // #error
    // #endif
    //                     return Keys[y][x];
    //                 x++;
    //             }
    //         }
    //         return Keys[0][0];
    //     }
    static void show(const char *wname)
    {

        if (!::PHONE)
            return;

        // static ImGuiOnceUponAFrame oaf;
        // if (!oaf)
        //     return;

        // ImGui::uponaframe

        static ImGuiID mywid = 0;
        static ImGuiID lastid = 0;
        static ImGuiID lastid_w = 0;

        ImGuiContext &g = *ImGui::GetCurrentContext();
        // ImGui::is
        if (g.ActiveId != 0x0 && ImGui::FindWindowByName(wname)->ID != 0)
        {
            if (ImGui::FindWindowByName(wname)->ID != g.ActiveIdWindow->ID && g.ActiveIdTimer > 0.5)
            {
                // ImGui::Find
                lastid = g.ActiveId;
                lastid_w = g.ActiveIdWindow->ID;
            }
        }
        static int shitstate = 0;
        static int editingHotkey = -1;

        static bool keyDown[512] = {};

        ImGui::SetNextWindowSize(ImVec2(340, 0));
        ImGuiWindowClass window_class;
        // window_class.DockNodeFlagsOverrideSet = ImGuiViewportFlags_NoFocusOnClick|ImGuiViewportFlags_NoTaskBarIcon|ImGuiViewportFlags_TopMost|ImGuiViewportFlags_NoAutoMerge;
        window_class.ViewportFlagsOverrideSet = ImGuiViewportFlags_TopMost | ImGuiViewportFlags_NoFocusOnClick | ImGuiViewportFlags_NoTaskBarIcon | ImGuiViewportFlags_TopMost | ImGuiViewportFlags_NoAutoMerge;
        ImGui::SetNextWindowClass(&window_class);
        if (!ImGui::Begin(wname, NULL, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoResize))
        {
            ImGui::End();
            return;
        }
        // ImGui::Text("mywid: %u", mywid);
        // ImGui::Text("lastid: %u", lastid);
        // ImGui::Text("lastid_w: %u", lastid_w);

        // ImGui::Text("WINDOWING");
        // ImGui::Indent();
        // ImGui::Text("HoveredWindow: '%s'", g.HoveredWindow ? g.HoveredWindow->Name : "NULL");
        // ImGui::Text("HoveredWindow->Root: '%s'", g.HoveredWindow ? g.HoveredWindow->RootWindowDockTree->Name : "NULL");
        // ImGui::Text("HoveredWindowUnderMovingWindow: '%s'", g.HoveredWindowUnderMovingWindow ? g.HoveredWindowUnderMovingWindow->Name : "NULL");
        // ImGui::Text("HoveredDockNode: 0x%08X", g.DebugHoveredDockNode ? g.DebugHoveredDockNode->ID : 0);
        // ImGui::Text("MovingWindow: '%s'", g.MovingWindow ? g.MovingWindow->Name : "NULL");
        // ImGui::Text("MouseViewport: 0x%08X (UserHovered 0x%08X, LastHovered 0x%08X)", g.MouseViewport->ID, g.IO.MouseHoveredViewport, g.MouseLastHoveredViewport ? g.MouseLastHoveredViewport->ID : 0);
        // ImGui::Unindent();

        // ImGui::Indent();
        // ImGui::Text("ITEMS");
        // ImGui::Text("ActiveId: 0x%08X/0x%08X (%.2f sec), AllowOverlap: %d, Source: %s", g.ActiveId, g.ActiveIdPreviousFrame, g.ActiveIdTimer, g.ActiveIdAllowOverlap, GetInputSourceName(g.ActiveIdSource));
        // ImGui::DebugLocateItemOnHover(g.ActiveId);
        // ImGui::Text("ActiveIdWindow: '%s'", g.ActiveIdWindow ? g.ActiveIdWindow->Name : "NULL");
        // ImGui::Text("ActiveIdUsing: AllKeyboardKeys: %d, NavDirMask: %X", g.ActiveIdUsingAllKeyboardKeys, g.ActiveIdUsingNavDirMask);
        // ImGui::Text("HoveredId: 0x%08X (%.2f sec), AllowOverlap: %d", g.HoveredIdPreviousFrame, g.HoveredIdTimer, g.HoveredIdAllowOverlap); // Not displaying g.HoveredId as it is update mid-frame
        // ImGui::Text("HoverDelayId: 0x%08X, Timer: %.2f, ClearTimer: %.2f", g.HoverDelayId, g.HoverDelayTimer, g.HoverDelayClearTimer);
        // ImGui::Text("DragDrop: %d, SourceId = 0x%08X, Payload \"%s\" (%d bytes)", g.DragDropActive, g.DragDropPayload.SourceId, g.DragDropPayload.DataType, g.DragDropPayload.DataSize);
        // ImGui::DebugLocateItemOnHover(g.DragDropPayload.SourceId);
        // ImGui::Unindent();

        // ImGui::Text("NAV,FOCUS");
        // ImGui::Indent();
        // ImGui::Text("NavWindow: '%s'", g.NavWindow ? g.NavWindow->Name : "NULL");
        // ImGui::Text("NavId: 0x%08X, NavLayer: %d", g.NavId, g.NavLayer);
        // ImGui::DebugLocateItemOnHover(g.NavId);
        // ImGui::Text("NavInputSource: %s", GetInputSourceName(g.NavInputSource));
        // ImGui::Text("NavActive: %d, NavVisible: %d", g.IO.NavActive, g.IO.NavVisible);
        // ImGui::Text("NavActivateId/DownId/PressedId/InputId: %08X/%08X/%08X/%08X", g.NavActivateId, g.NavActivateDownId, g.NavActivatePressedId, g.NavActivateInputId);
        // ImGui::Text("NavActivateFlags: %04X", g.NavActivateFlags);
        // ImGui::Text("NavDisableHighlight: %d, NavDisableMouseHover: %d", g.NavDisableHighlight, g.NavDisableMouseHover);
        // ImGui::Text("NavFocusScopeId = 0x%08X", g.NavFocusScopeId);
        // ImGui::Text("NavWindowingTarget: '%s'", g.NavWindowingTarget ? g.NavWindowingTarget->Name : "NULL");
        // ImGui::Unindent();
        // ImGui::SameLine();

        ImGui::BeginGroup();

        for (int i = 0; i < 512; i++)
        {
            if (ImGui::IsKeyPressed((ImGuiKey)i, false))
            {
                int imKey;
#ifdef SDL_h_
                imKey = i;
#elif WIN32
                imKey = MapVirtualKeyA(i, MAPVK_VK_TO_VSC);
#else
                imKey = i;
#endif
                keyDown[imKey] = !keyDown[imKey];
            }
        }
        for (unsigned int y = 0; y < 5; y++)
        {
            int x = 0;
            ImGui::BeginGroup();
            while (Keys[y][x].lib)
            {
                Key &key = Keys[y][x];
                const float ofs = key.offset / 2 + (x ? 2.f : 0.f);

                const float width = key.width / 2;
                if (x)
                {
                    ImGui::SameLine(0.f, ofs);
                }
                else
                {
                    if (ofs >= 1.f)
                    {
                        ImGui::Indent(ofs);
                    }
                }
#ifdef SDL_h_
                bool &butSwtch = keyDown[key.scanCodePage7];
#elif WIN32
                bool &butSwtch = keyDown[key.scanCodePage1];
#else
#error
#endif
                ImGui::PushStyleColor(ImGuiCol_Button, key.hold ? 0xFF1040FF : 0x80000000);
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
                if (ImGui::Button(Keys[y][x].lib, ImVec2(width, 20)))
                {
                    ImGui::SetActiveID(lastid, ImGui::FindWindowByID(lastid_w));

                    // ImGui::GetIO().AddKeyEvent((ImGuiKey)key.scanCodePage7,true);
                    // ImGui::GetIO().AddKeyEvent((ImGuiKey)key.scanCodePage7,false);
                    if (strlen(key.lib) == 1)
                    {
                        if (key.scanCodePage7 == 0x2A)
                        {
                            ImGui::GetIO().AddKeyEvent(ImGuiKey_Backspace, true);
                            ImGui::GetIO().AddKeyEvent(ImGuiKey_Backspace, false);
                        }
                        else if (key.scanCodePage7 == 0X28)
                        {
                            ImGui::GetIO().AddKeyEvent(ImGuiKey_KeypadEnter, true);
                            ImGui::GetIO().AddKeyEvent(ImGuiKey_KeypadEnter, false);
                        }
                        else
                            ImGui::GetIO().AddInputCharacter(shitstate > 0 ? toupper(*key.lib) : tolower(*key.lib));
                    }   
                    if (key.scanCodePage7 == 0X2c)
                    {
                        ImGui::GetIO().AddInputCharacter(shitstate > 0 ? toupper(' ') : tolower(' '));
                    }

                    if (shitstate == 1)
                        shitstate = 0;

                    Keys[3][0].hold = false;
                    Keys[3][11].hold = false;
                    if (key.scanCodePage7 == 0xE1 || key.scanCodePage7 == 0xE5)
                    { // shift
                        key.hold = !key.hold;
                        shitstate = 1;
                    }

                    if (key.scanCodePage7 == 0x39)
                    { // caps
                        key.hold = !key.hold;
                        shitstate = key.hold ? 2 : 0;
                    }

                    ImGui::DebugLocateItem(lastid);
                    ImGui::GetForegroundDrawList(g.CurrentWindow)->AddRect(g.LastItemData.Rect.Min - ImVec2(3.0f, 3.0f), g.LastItemData.Rect.Max + ImVec2(3.0f, 3.0f), IM_COL32(0, 255, 0, 255));
                }
                // if(ImGui::IsItemClicked((ImGuiMouseButton)0)){
                //     if (key.hold == false)
                //     {
                //         ImGui::SetActiveID(lastid, ImGui::FindWindowByID(lastid_w));
                //         ImGui::GetIO().AddKeyEvent((ImGuiKey)key.scanCodePage1,true);

                //     }

                //     key.hold = true;

                // }else if( key.hold){
                //     ImGui::GetIO().AddKeyEvent((ImGuiKey)key.scanCodePage1,false);
                // }
                ImGui::PopFont();
                ImGui::PopStyleColor();
                x++;
            }
            ImGui::EndGroup();
        }

        // ImGui::InvisibleButton("space", ImVec2(10, 55));
        // ImGui::BeginChildFrame(18, ImVec2(540, 40));
        // ImGui::Text("%s :", hotkey[editingHotkey].functionName);
        // ImGui::SameLine();
        // ImGui::TextWrapped("%s", hotkey[editingHotkey].functionLib);
        // ImGui::EndChildFrame();
        // ImGui::SameLine();
        // int keyDownCount = 0;
        // for (auto d : keyDown)
        // {
        //     keyDownCount += d ? 1 : 0;
        // }
        // if (ImGui::Button("Clear", ImVec2(80, 40)))
        // {
        //     memset(keyDown, 0, sizeof(keyDown));
        // }
        // ImGui::SameLine();
        // if (keyDownCount && keyDownCount < 5)
        // {
        //     if (ImGui::Button("Set", ImVec2(80, 40)))
        //     {
        //         unsigned char scanCodes[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
        //         unsigned char order[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
        //         int scanCodeCount = 0;
        //         hotkey[editingHotkey].functionKeys = 0;
        //         for (int i = 1; i < sizeof(keyDown); i++)
        //         {
        //             if (keyDown[i])
        //             {
        //                 scanCodes[scanCodeCount] = (unsigned char)i;
        //                 order[scanCodeCount] = (unsigned char)GetKeyForScanCode(i).order;
        //                 scanCodeCount++;
        //             }
        //         }

        //         hotkey[editingHotkey].functionKeys = GetOrderedScanCodes(scanCodes, order);
        //     }
        //     ImGui::SameLine(0.f, 20.f);
        // }
        // else
        // {
        //     ImGui::SameLine(0.f, 100.f);
        // }

        // if (ImGui::Button("Done", ImVec2(80, 40))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndGroup();
        ImGui::End();
    }
};