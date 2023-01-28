#include "csv.hpp"

#include "exception.hpp"

#include <fstream>
#include <regex>

namespace csv {

namespace {

std::vector<std::string>
split(const std::string& line, const char separator)
{
  std::vector<std::string> fields;

  std::string field;

  for (size_t i = 0; i < line.size(); i++) {
    const char c = line[i];
    if (c == separator) {
      fields.emplace_back(field);
      field.clear();
    } else {
      field.push_back(c);
    }
  }

  fields.emplace_back(std::move(field));

  return fields;
}

} // namespace

void
file::load(const char* path_parameter, const char separator)
{
  path = path_parameter;

  std::ifstream file(path);

  if (!file.good())
    throw file_open_exception("Failed to open file.", path);

  std::string line;

  if (!std::getline(file, line))
    throw file_open_exception("Failed to read header.", path);

  auto headers = split(line, separator);

  columns.resize(headers.size());

  for (size_t i = 0; i < columns.size(); i++) {

    if (headers[i].empty())
      throw file_open_exception("Found header column with empty name.", path);

    columns[i].name = std::move(headers[i]);

    columns[i].data.clear();
  }

  row_count = 0;

  while (file) {

    if (!std::getline(file, line))
      break;

    if (line.empty())
      continue;

    auto fields = split(line, separator);

    if (fields.size() != headers.size())
      throw file_open_exception("Found row with missing fields.", path);

    for (size_t i = 0; i < fields.size(); i++) {
      if (fields[i].empty())
        throw file_open_exception("Found field with an empty string.", path);

      columns[i].data.emplace_back(std::move(fields[i]));
    }

    ++row_count;
  }
}

namespace {

bool
try_match(const column& c, const std::string& pattern)
{
  std::regex r(pattern.c_str(), pattern.size(), std::regex_constants::ECMAScript | std::regex_constants::icase);

  for (const auto& row : c.data) {
    if (!std::regex_match(row, r))
      return false;
  }

  return true;
}

} // namespace

column_type
column::guess_column_type() const
{
  auto type = column_type::unknown;

  if (try_match(*this, "[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?"))
    return column_type::numerical;

  return type;
}

} // namespace csv
