#include "application.hpp"

#include "menu_bar.hpp"
#include "file_dialog.hpp"
#include "csv.hpp"
#include "data_set.hpp"
#include "session.hpp"

#include "font.hpp"

#include <mlpack/core.hpp>

#include <glad/glad.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <implot.h>

namespace {

class load_job final
{
public:
  using load_func = void (*)(void*);

  using completion_callback = void(*)(void*, std::unique_ptr<data_set>);

  explicit load_job(std::string path, load_func f, void* caller, completion_callback completion_cb)
    : path_(std::move(path))
      , thread_(f, this)
      , caller_(caller)
      , completion_callback_(completion_cb)
  {
  }

  const std::string& get_path() const { return path_; }

  bool poll()
  {
    if (complete_.load()) {
      if (thread_.joinable())
        thread_.join();
      completion_callback_(caller_, std::move(data_set_));
      return true;
    }

    return false;
  }

  static void load_csv_func(void* self_ptr)
  {
    auto* self = static_cast<load_job*>(self_ptr);

    csv::file file;

    file.load(self->path_.c_str());

    self->data_set_ = std::make_unique<csv_data_set>(file);

    self->complete_.store(true);
  }

private:
  std::string path_;

  std::atomic<bool> complete_;

  std::unique_ptr<data_set> data_set_;

  std::thread thread_;

  void* caller_{ nullptr };

  completion_callback completion_callback_{ nullptr };
};

class chart_renderer final : public data_set_visitor
{
public:
  void visit(const csv_data_set& ds) override
  {
    if (ImPlot::BeginPlot(ds.get_name())) {

      const auto& data = ds.get_data();

      for (size_t i = 0; i < ds.get_column_count(); i++) {

        const char* name = ds.get_column_name(i);

        const float* y = data.colptr(i);

        ImPlot::PlotLine(name, y, static_cast<int>(data.n_rows));
      }

      ImPlot::EndPlot();
    }
  }

  void visit(const img_data_set&) override
  {
  }
};

class data_set_info_renderer final : public data_set_visitor
{
public:
  void visit(const csv_data_set& ds) override
  {
    if (ImGui::CollapsingHeader(ds.get_name())) {

      const auto column_count = ds.get_column_count();

      for (size_t i = 0; i < column_count; ++i) {

        const char* column_name = ds.get_column_name(i);

        if (ImGui::CollapsingHeader(column_name)) {

        }
      }

      render_close_button(&ds);
    }
  }

  void visit(const img_data_set& ds) override
  {
    if (ImGui::CollapsingHeader(ds.get_name())) {

      render_close_button(&ds);
    }
  }

  std::vector<std::unique_ptr<session::operation>> take_operations()
  {
    return std::move(operations_);
  }

private:
  void render_close_button(const data_set* ds)
  {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.0, 1.0));

    if (ImGui::Button("Close")) {

      operations_.emplace_back(std::make_unique<session::close_data_set_operation>(ds));
    }

    ImGui::PopStyleColor(1);
  }

private:
  std::vector<std::unique_ptr<session::operation>> operations_;
};

class application_impl final : public application
{
public:
  explicit application_impl(GLFWwindow* window)
    : window_(window)
  {
    auto& style = ImGui::GetStyle();
    style.ChildBorderSize = 0;
    style.FrameBorderSize = 0;
    style.WindowBorderSize = 0;
    style.PopupBorderSize = 1;

    style.ChildRounding = 4;
    style.FrameRounding = 4;
    style.WindowRounding = 4;
    style.PopupRounding = 4;
  }

  bool run() override
  {
    while (!glfwWindowShouldClose(window_)) {

      glfwPollEvents();

      if (font_rebuild_queued_) {
        rebuild_fonts();
        font_rebuild_queued_ = false;
      }

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
      if (data_load_dialog_->poll())
        data_load_dialog_.reset();
    }

    while (true) {

      bool completed{ false };

      for (auto it = load_jobs_.begin(); it != load_jobs_.end(); ++it) {
        if ((*it)->poll()) {
          load_jobs_.erase(it);
          completed = true;
          break;
        }
      }

      if (!completed)
        break;
    }
  }

  static void on_csv_load_complete(void* self_ptr, std::unique_ptr<data_set> ds)
  {
    auto* self{ static_cast<application_impl*>(self_ptr) };

    self->session_.add_data_set(std::move(ds));
  }

  static void on_csv_load_request(void* self_ptr, const std::vector<std::string>& paths)
  {
    auto* self{ static_cast<application_impl*>(self_ptr) };

    for (const auto& path : paths) {

      auto job{ std::make_unique<load_job>(path, load_job::load_csv_func, self_ptr, on_csv_load_complete) };

      self->load_jobs_.emplace_back(std::move(job));
    }
  }

  static void on_tsv_load(void* self_ptr, const std::vector<std::string>& paths)
  {
  }

  void load_csv_data_file(const char* title, const char* extension, const file_dialog::callback callback)
  {
    if (data_load_dialog_)
      return;

    data_load_dialog_ = file_dialog::create_data_load_dialog(title, extension, this, callback);
  }

  void render_frame()
  {
    ImGui::PushFont(font_);

    switch (menu_bar_.render()) {
      case menu_bar::action::exit:
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
        break;
      case menu_bar::action::new_chart:
        session_.new_chart();
        break;
      case menu_bar::action::load_csv:
        load_csv_data_file("CSV Files", "*.csv", on_csv_load_request);
        break;
      case menu_bar::action::load_tsv:
        load_csv_data_file("TSV Files", "*.tsv", on_tsv_load);
        break;
      case menu_bar::action::undo:
        session_.undo();
        break;
      case menu_bar::action::redo:
        session_.redo();
        break;
      case menu_bar::action::change_font_size:
        font_rebuild_queued_ = true;
        break;
      case menu_bar::action::none:
        break;
    }

    const auto menu_size = menu_bar_.size();

    const auto max_size = ImGui::GetIO().DisplaySize;

    const auto side_panel_offset = menu_bar_.side_panel_size();

    const auto margin = menu_bar_.margin();

    const auto separation_margin = menu_bar_.side_panel_visible() ? margin : 0;

    // render side panel

    ImGui::SetNextWindowPos(ImVec2(margin, menu_size.y + margin), ImGuiCond_Always);

    ImGui::SetNextWindowSize(ImVec2(side_panel_offset, max_size.y - menu_size.y - (margin * 2)), ImGuiCond_Always);

    if (menu_bar_.side_panel_visible()) {

      ImGui::Begin("Data Set Panel", nullptr, ImGuiWindowFlags_NoDecoration);

      render_side_panel();

      ImGui::End();
    }

    // render tabs

    ImGui::SetNextWindowPos(ImVec2(side_panel_offset + (margin * 1) + separation_margin, menu_size.y + margin),
                            ImGuiCond_Always);

    ImGui::SetNextWindowSize(
      ImVec2(max_size.x - (side_panel_offset + (margin * 2) + separation_margin),
             max_size.y - menu_size.y - (margin * 2)),
      ImGuiCond_Always);

    ImGui::Begin("Tabs Window", nullptr, ImGuiWindowFlags_NoDecoration);

    if (ImGui::BeginTabBar("Tabs")) {

      if (ImGui::BeginTabItem("Visuals")) {

        render_visuals();

        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Log")) {

        ImGui::EndTabItem();
      }

      ImGui::EndTabBar();
    }

    ImGui::End();

    ImGui::PopFont();
  }

  static bool render_spinner(const char* label, float radius, int thickness, const ImU32& color)
  {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
      return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
      return false;

    // Render
    window->DrawList->PathClear();

    int num_segments = 30;
    int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

    const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

    const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

    for (int i = 0; i < num_segments; i++) {
      const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
      window->DrawList->PathLineTo(
        ImVec2(centre.x + ImCos(a + g.Time * 8) * radius, centre.y + ImSin(a + g.Time * 8) * radius));
    }

    window->DrawList->PathStroke(color, false, thickness);

    return true;
  }

  void render_charts()
  {
    const auto* m = session_.get_model();

    for (const auto& chart : m->charts) {

      if (ImGui::CollapsingHeader(chart.second.name.c_str())) {

        auto hide_mouse_text{ chart.second.hide_mouse_text };

        auto cross_hairs{ true };

        if (ImGui::Checkbox("Crosshairs", &cross_hairs)) {

        }

        if (ImGui::Checkbox("Hide Mouse Text", &hide_mouse_text)) {
        }
      }
    }
  }

  void render_side_panel()
  {
    if (ImGui::BeginTabBar("Side Panel Tabs")) {

      if (ImGui::BeginTabItem("Datasets")) {

        data_set_info_renderer renderer;

        session_.accept_data_set_visitor(renderer);

        auto ops = renderer.take_operations();

        for (auto& op : ops)
          session_.apply_operation(*op);

        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Charts")) {

        render_charts();

        ImGui::EndTabItem();
      }

      if (!load_jobs_.empty())
        ImGui::Separator();

      for (size_t i = 0; i < load_jobs_.size(); i++) {

        const auto& job = load_jobs_[i];

        ImGui::PushID(i);

        render_spinner("Loading", menu_bar_.font_size() * 0.5f, 2, ImGui::GetColorU32(ImGuiCol_Button));

        ImGui::PopID();

        ImGui::SameLine();

        ImGui::Text("%s", job->get_path().c_str());
      }

      ImGui::EndTabBar();
    }
  }

  void render_visuals() const
  {
    const auto* m = session_.get_model();

    for (const auto& c : m->charts) {

      if (ImPlot::BeginPlot(c.second.name.c_str())) {

        ImPlot::EndPlot();
      }
    }

    chart_renderer renderer;

    session_.accept_data_set_visitor(renderer);
  }

  void rebuild_fonts()
  {
    font_ = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
      RobotoMedium_compressed_data,
      RobotoMedium_compressed_size,
      menu_bar_.font_size());

    ImGui::GetIO().Fonts->Build();

    ImGui_ImplOpenGL3_DestroyFontsTexture();

    ImGui_ImplOpenGL3_CreateFontsTexture();
  }

private:
  session session_;

  GLFWwindow* window_{ nullptr };

  std::unique_ptr<file_dialog> data_load_dialog_;

  std::vector<std::unique_ptr<load_job>> load_jobs_;

  menu_bar menu_bar_;

  bool font_rebuild_queued_{ true };

  ImFont* font_{ nullptr };
};

} // namespace

std::unique_ptr<application>
application::create(GLFWwindow* window)
{
  return std::make_unique<application_impl>(window);
}
