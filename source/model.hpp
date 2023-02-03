#pragma once

#include <vector>
#include <string>
#include <map>

/// The data model of the visualization project.
struct model final
{
  /// Used for generating unique IDs within the data model.
  class id_generator final
  {
  public:
    /// Constructs a new ID generator.
    ///
    /// @param id The value of the first ID.
    explicit id_generator(const int id = 0)
      : next_id_(id)
    {
    }

    /// Generates a new ID.
    int operator()() { return next_id_++; }

  private:
    /// The value to be assigned to the next ID.
    int next_id_{ 0 };
  };

  /// The data model for a CSV file.
  struct csv_data_set final
  {
    /// The path to the CSV file.
    std::string path;
  };

  /// The model for a chart.
  struct chart final
  {
    /// The name of the chart.
    std::string name;

    /// Whether or not to hide the title.
    bool hide_title{ false };

    /// Hides the mouse text appearing on the lower right.
    bool hide_mouse_text{ false };

    /// Whether or not to show cross hairs.
    bool crosshairs{ true };
  };

  /// The CSV data sets loaded by the user.
  std::map<int, csv_data_set> csv_data_sets;

  /// The charts added to the project.
  std::map<int, chart> charts;

  /// Used for generating unique IDs for various resources.
  id_generator id_generator_;

  /// Generates a new ID.
  ///
  /// @return A newly generated ID.
  int generate_id() { return id_generator_(); }

  bool load(const char* path);

  bool save(const char* path) const;
};
