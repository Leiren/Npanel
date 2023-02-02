#pragma once
#include "imgui.h"
#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
void ImageRotated(ImTextureID tex_id, ImVec2 center, ImVec2 size, float angle);
