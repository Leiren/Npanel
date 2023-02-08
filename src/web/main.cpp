#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include <iostream>
#include "implot.h"
#include <cstdio>
#include <iostream>
#include "imnodes.h"

#if defined(__EMSCRIPTEN__)
#include <SDL_opengles2.h>
#include <emscripten.h>

#elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h> // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h> // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h> // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

extern void loop();
extern void setup();
SDL_Window *g_Window = NULL;
SDL_GLContext g_GLContext = NULL;



int init_gl()
{

  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
  {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
  // GL 3.2 Core + GLSL 150
  const char *glsl_version = "#version 150";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  g_Window = SDL_CreateWindow("Npanel", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, window_flags);
  g_GLContext = SDL_GL_CreateContext(g_Window);
  SDL_GL_MakeCurrent(g_Window, g_GLContext);
  SDL_GL_SetSwapInterval(1); // Enable vsync

  return 0;
}
extern void apply_theme_default();
int init_imgui()
{

  // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
#elif defined(__APPLE__)
  // GL 3.2 Core + GLSL 150
  const char *glsl_version = "#version 150";
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";

#endif
  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImNodes::CreateContext();

  // Setup style
  ImGui::StyleColorsDark();
  apply_theme_default();
  ImPlot::StyleColorsAuto();
  ImNodes::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(g_Window, g_GLContext);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = NULL;
  // enable docking
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // Load Fonts
  const char *font = "resources/Poppins-Medium.ttf";
  io.Fonts->AddFontFromFileTTF(font, 23.0f);
  io.Fonts->AddFontFromFileTTF(font, 18.0f);
  io.Fonts->AddFontFromFileTTF(font, 26.0f);
  io.Fonts->AddFontFromFileTTF(font, 32.0f);
  io.Fonts->AddFontDefault();

  return 0;
}

int init()
{

  init_gl();
  init_imgui();
  setup();
  // printf("hello, world!\n");

  // freopen("recousrces/ggout.txt","w",stdout);
  // std::cout << "hi";

  return 0;
}

void quit()
{
  // glfwTerminate();
}
extern bool g_done;

extern "C" int main(int argc, char **argv)
{
  if (init() != 0)
    return 1;

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
#endif
  while (!g_done)
    loop();

  quit();

  return 0;
}