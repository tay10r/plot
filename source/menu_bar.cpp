#include "menu_bar.hpp"

menu_bar::action
menu_bar::render(ImVec2* size)
{
  auto action = menu_bar::action::none;

  if (!ImGui::BeginMainMenuBar())
    return action;

  *size = ImGui::GetWindowSize();

  if (ImGui::BeginMenu("File")) {

    if (ImGui::MenuItem("Load Data"))
      action = menu_bar::action::load_file;

    ImGui::Separator();

    if (ImGui::MenuItem("Exit"))
      action = menu_bar::action::exit;

    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Help")) {
    ImGui::EndMenu();
  }

  ImGui::EndMainMenuBar();

  return action;
}
