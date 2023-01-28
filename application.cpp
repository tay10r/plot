#include "application.hpp"

#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace {

class application_impl final : public application
{
public:
  explicit application_impl(GLFWwindow* window)
    : window_(window)
  {
  }

  bool run() override
  {
    while (!glfwWindowShouldClose(window_)) {

      glfwPollEvents();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      render_frame();

      ImGui::Render();

      int display_w{ 0 }, display_h{ 0 };

      glfwGetFramebufferSize(window_, &display_w, &display_h);

      glViewport(0, 0, display_w, display_h);

      glClearColor(0, 0, 0, 1);

      glClear(GL_COLOR_BUFFER_BIT);

      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window_);
    }

    return true;
  }

private:
  ImVec2 render_main_menu_bar()
  {
    ImVec2 size{ 0, 0 };

    if (ImGui::BeginMainMenuBar()) {

      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Open File")) {
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Exit")) {
          glfwSetWindowShouldClose(window_, GLFW_TRUE);
        }

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Help")) {
        ImGui::EndMenu();
      }

      size = ImGui::GetWindowSize();

      ImGui::EndMainMenuBar();
    }

    return size;
  }

  void render_frame()
  {
    auto main_menu_bar_size{ render_main_menu_bar() };

    (void)main_menu_bar_size;
    //
  }

private:
  GLFWwindow* window_{ nullptr };
};

} // namespace

std::unique_ptr<application>
application::create(GLFWwindow* window)
{
  return std::unique_ptr<application>(new application_impl(window));
}
