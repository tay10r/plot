#include "application.hpp"

#include "menu_bar.hpp"
#include "file_dialog.hpp"

#include <mlpack/core.hpp>

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

      check_io();

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
  void check_io()
  {
    if (data_load_dialog_) {
      if (data_load_dialog_->ready()) {
        auto results = data_load_dialog_->results();
        data_load_dialog_.reset();
      }
    }
  }

  void load_data_file()
  {
    if (data_load_dialog_)
      return;

    data_load_dialog_ = file_dialog::create_data_load_dialog();
  }

  void render_frame()
  {
    ImVec2 menu_size{ 0, 0 };

    switch (menu_bar::render(&menu_size)) {
      case menu_bar::action::exit:
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
        break;
      case menu_bar::action::load_file:
        load_data_file();
        break;
      case menu_bar::action::none:
        break;
    }

    ImGui::SetNextWindowPos(ImVec2(0, menu_size.y), ImGuiCond_Always);

    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - menu_size.y),
                             ImGuiCond_Always);

    ImGui::Begin("Main Window", nullptr, ImGuiWindowFlags_NoDecoration);

    if (ImGui::BeginTabBar("Tabs")) {

      if (ImGui::BeginTabItem("Data")) {

        ImGui::EndTabItem();
      }

#if 0
      if (ImGui::BeginTabItem("Models")) {

        ImGui::EndTabItem();
      }
#endif

      if (ImGui::BeginTabItem("Log")) {

        ImGui::EndTabItem();
      }

      ImGui::EndTabBar();
    }

    ImGui::End();
  }

private:
  GLFWwindow* window_{ nullptr };

  std::unique_ptr<file_dialog> data_load_dialog_;
};

} // namespace

std::unique_ptr<application>
application::create(GLFWwindow* window)
{
  return std::make_unique<application_impl>(window);
}
