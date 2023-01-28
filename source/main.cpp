#ifdef _WIN32
#include <Windows.h>
#endif

#include <GLFW/glfw3.h>

#include <glad/glad.h>

#include <mlpack/core.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdlib>

#include "application.hpp"

namespace {

bool
run(GLFWwindow* window)
{
  glfwMakeContextCurrent(window);

  glfwSwapInterval(1);

  gladLoadGLES2Loader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

  IMGUI_CHECKVERSION();

  ImGui::CreateContext();

  ImGui_ImplGlfw_InitForOpenGL(window, true);

  ImGui_ImplOpenGL3_Init("#version 100");

  ImGui::GetIO().IniFilename = nullptr;

  auto app = application::create(window);

  const auto success{ app->run() };

  ImGui_ImplOpenGL3_Shutdown();

  ImGui_ImplGlfw_Shutdown();

  ImGui::DestroyContext();

  return success;
}

void
glfw_error_callback(int, const char* description)
{
  mlpack::Log::Warn << "GLFW error occurred: " << description << std::endl;
}

} // namespace

#ifdef _WIN32
int
WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
int
main()
#endif
{
  glfwSetErrorCallback(glfw_error_callback);

  if (glfwInit() != GLFW_TRUE)
    return EXIT_FAILURE;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  const auto success{ run(window) };

  glfwDestroyWindow(window);

  glfwTerminate();

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
