#include "file_dialog.hpp"

#include <portable-file-dialogs.h>

namespace {

class data_load_dialog final : public file_dialog
{
public:
  data_load_dialog()
    : impl_("Load Data",
            "",
            { "CSV Files",
              "*.csv",
              "TSV Files",
              "*.tsv",
              "PPM Images",
              "*.ppm",
              "PGM Images",
              "*.pgm",
              "HDF5 Files",
              "*.hdf5" },
            pfd::opt::multiselect)
  {
  }

  [[nodiscard]] bool ready() const override
  {
    return impl_.ready(0);
  }

  [[nodiscard]] std::vector<std::string> results() override
  {
    return impl_.result();
  }

private:
  pfd::open_file impl_;
};

} // namespace

std::unique_ptr<file_dialog>
file_dialog::create_data_load_dialog()
{
  return std::make_unique<data_load_dialog>();
}
