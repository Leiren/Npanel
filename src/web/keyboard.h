
#pragma once
/*
How to use it?

// Get some hotkeys composed of:
// - hotkey name
// - hotkey comment/lib
// - hotkey scancodes. Computed by the editor. Store that value in your app.

static std::vector<ImHotKey::HotKey> hotkeys = { { "Layout", "Reorder nodes in a simpler layout", 0xFFFF261D}
        ,{"Save", "Save the current graph", 0xFFFF1F1D}
        ,{"Load", "Load an existing graph file", 0xFFFF181D}
        ,{"Play/Stop", "Play or stop the animation from the current graph", 0xFFFFFF3F}
        ,{"SetKey", "Make a new animation key with the current parameters values at the current time", 0xFFFFFF1F}
        };

// The editor is a modal window. bring it with something like that
 if (ImGui::Button("Edit Hotkeys"))
{
    ImGui::OpenPopup("HotKeys Editor");
}
ImHotKey::Edit(hotkeys.data(), hotkeys.size(), "HotKeys Editor");

// ImHotKey also provides a way to retrieve HotKey
int hotkey = ImHotKey::GetHotKey(hotkeys.data(), hotkeys.size());
if (hotkey != -1)
{
    // handle the hotkey index!
}

Awesome, you are done!
To help you integrate in your app, you can get a text (like "Ctrl + O") to integrate in your menu
static void GetHotKeyLib(unsigned int functionKeys, char* buffer, size_t bufferSize);

*/

#include "imgui.h"
#include "imgui_internal.h"

namespace ImHotKey
{
    struct HotKey
    {
        const char *functionName;
        const char *functionLib;
        unsigned int functionKeys;
    };

    struct Key
    {
        const char* lib = nullptr;
        unsigned int order;
        unsigned int scanCodePage1 = 0; // win32 scancode
        unsigned int scanCodePage7 = 0; // HID (SDL,...)
        float offset = 0;
        float width = 40;
    };

    static const Key Keys[5][18] = {
      
        { {"~", 20, 0x29, 0x35}, {"1", 21, 0x2, 0x1E}, {"2", 22, 0x3, 0x1F}, {"3", 23, 0x4, 0x20}, {"4", 24, 0x5, 0x21}, {"5", 25, 0x6, 0x22}, {"6", 26, 0x7, 0x23}, {"7", 27, 0x8, 0x24}, {"8", 28, 0x9, 0x25}, {"9", 29, 0xA, 0x26}, {"0", 30, 0xB, 0x27}, {"-", 31, 0xC, 0x2D}, {"+", 32, 0xD, 0x2E},{"<", 33, 0xE, 0x2A, 0, 80} },
        { {"Tab", 3, 0xF, 0x2B, 0, 60}, {"Q", 37, 0x10, 0x14}, {"W", 38, 0x11, 0x1A}, {"E", 39, 0x12, 0x08}, {"R", 40, 0x13, 0x15}, {"T", 41, 0x14, 0x17}, {"Y", 42, 0x15, 0x1C}, {"U", 43, 0x16, 0x18}, {"I", 44, 0x17, 0x0C}, {"O", 45, 0x18, 0x12}, {"P", 46, 0x19, 0x13}, {"[", 47, 0x1A, 0x2F}, {"]", 48, 0x1B, 0x30}, {"|", 49, 0x2B, 0x31, 0, 60}},
        { {"Caps Lock", 53, 0x3A, 0x39, 0, 80}, {"A", 54, 0x1E, 0x04}, {"S", 55, 0x1F, 0x16}, {"D", 56, 0x20, 0x07}, {"F", 57, 0x21, 0x09}, {"G", 58, 0x22, 0x0A}, {"H", 59, 0x23, 0x0B}, {"J", 60, 0x24, 0x0D}, {"K", 61, 0x25, 0x0E}, {"L", 62, 0x26, 0x0F}, {";", 63, 0x27, 0x33}, {"'", 64, 0x28, 0x34}, {"Ret", 65, 0x1C, 0X28, 0, 84} },
        { {"Shift", 2, 0x2A, 0xE1, 0, 104}, {"Z", 66, 0x2C, 0x1D}, {"X", 67, 0x2D, 0x1B}, {"C", 68, 0x2E, 0x06}, {"V", 69, 0x2F, 0x19}, {"B", 70, 0x30, 0x05}, {"N", 71, 0x31, 0x11}, {"M", 72, 0x32, 0x10}, {",", 73, 0x33, 0x36}, {".", 74, 0x34, 0x37}, {"/", 75, 0x35, 0x38}, {"Shift", 2, 0x2A, 0xE5, 0, 104}, },
        { {"Ctrl", 0, 0x1D, 0xE0, 0, 60}, {"Alt", 1, 0x38, 0xE2, 68, 60}, {"Space", 77, 0x39, 0X2c, 0, 260}, {"Alt", 1, 0x38, 0xE6, 0, 60}, {"Ctrl", 0, 0x1D, 0xE4, 68, 60} }
    };

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


    static void Edit(const char *popupModal)
    {
        static int editingHotkey = -1;
        // if (!hotkeyCount)
        //     return;
        static bool keyDown[512] = {};

        ImGui::SetNextWindowSize(ImVec2(680, 275));
        // ImGuiWindowClass window_class;
        // window_class.ViewportFlagsOverrideClear = ImGuiViewportFlags_NoFocusOnClick|ImGuiViewportFlags_NoTaskBarIcon|ImGuiViewportFlags_TopMost|ImGuiViewportFlags_NoAutoMerge;
        // // window_class.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoFocusOnClick|ImGuiViewportFlags_NoTaskBarIcon|ImGuiViewportFlags_TopMost|ImGuiViewportFlags_NoAutoMerge;
        // ImGui::SetNextWindowClass(&window_class);
        if (!ImGui::Begin(popupModal, NULL,ImGuiWindowFlags_ChildWindow|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNavFocus|ImGuiWindowFlags_NoNavInputs| ImGuiWindowFlags_NoResize)){
            ImGui::End();
            return;

        }

        ImGui::SameLine();
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
                const Key& key = Keys[y][x];
                const float ofs = key.offset + (x?4.f:0.f);

                const float width = key.width;
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
                bool& butSwtch = keyDown[key.scanCodePage7];
#elif WIN32
                bool& butSwtch = keyDown[key.scanCodePage1];
#else
#error
#endif
                ImGui::PushStyleColor(ImGuiCol_Button, 0x80000000);
                if (ImGui::Button(Keys[y][x].lib, ImVec2(width, 40)))
                {
                    ImGui::GetIO().AddKeyEvent((ImGuiKey)(ImGuiKey_X),true);

                    // butSwtch = !butSwtch;
                }
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