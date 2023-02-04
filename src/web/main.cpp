#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include "implot.h"
#include <cstdio>
#include <iostream>
#include "imnodes.h"

extern void loop();
extern void setup();
GLFWwindow *g_window;

EM_JS(void, resizeCanvas, (), {
  js_resizeCanvas();
});

int init_gl()
{
  if (!glfwInit())
  {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return 1;
  }

  // glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

  // Open a window and create its OpenGL context
  int canvasWidth = 800;
  int canvasHeight = 600;
  g_window = glfwCreateWindow(canvasWidth, canvasHeight, "WebGui Demo", NULL, NULL);
  if (g_window == NULL)
  {
    fprintf(stderr, "Failed to open GLFW window.\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(g_window); // Initialize GLEW

  return 0;
}
extern void apply_theme_default();
int init_imgui()
{
  // Setup Dear ImGui binding
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImNodes::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(g_window, true);
  ImGui_ImplOpenGL3_Init();

  // Setup style
  ImGui::StyleColorsDark();
  apply_theme_default();
  ImPlot::StyleColorsAuto();
  ImNodes::StyleColorsDark();

  ImGuiIO &io = ImGui::GetIO();

  // Load Fonts
  const char *font = "resources/Poppins-Medium.ttf";
  io.Fonts->AddFontFromFileTTF(font, 23.0f);
  io.Fonts->AddFontFromFileTTF(font, 18.0f);
  io.Fonts->AddFontFromFileTTF(font, 26.0f);
  io.Fonts->AddFontFromFileTTF(font, 32.0f);
  io.Fonts->AddFontDefault();

  // enable docking
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  resizeCanvas();

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
  glfwTerminate();
}

extern "C" int main(int argc, char **argv)
{
  if (init() != 0)
    return 1;

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(loop, 0, 1);
#endif
  loop();

  quit();

  return 0;
}