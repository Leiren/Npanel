#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "colors.h"


void ToggleButton(const char *str_id, bool *v);
bool Spinner(const char *label, float radius, int thickness, const ImU32 &color);
void LoadingDialogFrame(bool show);
void HelpMarker(const char *desc);