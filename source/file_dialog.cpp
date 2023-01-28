#include "file_dialog.hpp"

#include <portable-file-dialogs.h>

namespace {

class data_load_dialog final : public file_dialog
{
public:
  data_load_dialog(const char* title, const char* ext, void* caller, const callback cb)
    : impl_("Load Data",
            "",
            {
              title,
              ext,
            },
            pfd::opt::multiselect)
      , caller_(caller)
      , callback_(cb)
  {
  }

  [[nodiscard]] bool poll() override
  {
    if (impl_.ready(0) && !done_) {
      callback_(caller_, impl_.result());
      done_ = true;
      return true;
    }

    return false;
  }

private:
  pfd::open_file impl_;

  bool done_{ false };

  void* caller_{ nullptr };

  const callback callback_{ nullptr };
};

} // namespace

std::unique_ptr<file_dialog>
file_dialog::create_data_load_dialog(const char* title, const char* ext, void* caller, const callback cb)
{
  return std::make_unique<data_load_dialog>(title, ext, caller, cb);
}
