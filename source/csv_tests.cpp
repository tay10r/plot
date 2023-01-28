#include <gtest/gtest.h>

#include "exception.hpp"
#include "csv.hpp"

#include <fstream>

#include <cstdio>

void
create_file(const std::string& path, const std::string& data)
{
  std::ofstream file(path);
  file << data;
}

TEST(csv, missing_file_should_throw)
{
  csv::file file;

  EXPECT_THROW(file.load("this_is_a_missing_file.csv"), file_open_exception);
}

TEST(csv, empty_file)
{
  csv::file file;

  create_file("empty_file.csv", "");

  EXPECT_THROW(file.load("empty_file.csv"), file_open_exception);

  (void)remove("empty_file.csv");
}

TEST(csv, empty_header)
{
  csv::file file;

  create_file("empty_header.csv", "a,\n");

  EXPECT_THROW(file.load("empty_header.csv"), file_open_exception);

  (void)remove("empty_header.csv");
}

TEST(csv, empty_field)
{
  csv::file file;

  create_file("empty_field.csv", "a,b\n0,\n");

  EXPECT_THROW(file.load("empty_field.csv"), file_open_exception);

  (void)remove("empty_field.csv");
}

TEST(csv, empty_line_okay)
{
  csv::file file;

  create_file("empty_line.csv", "a,b\n0,1\n\n2,3\n");

  EXPECT_NO_THROW(file.load("empty_line.csv"));

  (void)remove("empty_line.csv");

  EXPECT_EQ(file.columns.at(0).name, "a");
  EXPECT_EQ(file.columns.at(1).name, "b");

  EXPECT_EQ(file.columns.at(0).data.at(0), "0");
  EXPECT_EQ(file.columns.at(0).data.at(1), "2");

  EXPECT_EQ(file.columns.at(1).data.at(0), "1");
  EXPECT_EQ(file.columns.at(1).data.at(1), "3");
}

TEST(csv, load_default)
{
  csv::file file;

  create_file("test_data.csv", "a,b\n0,1\n2,3\n");

  file.load("test_data.csv");

  EXPECT_EQ(file.columns.at(0).name, "a");
  EXPECT_EQ(file.columns.at(1).name, "b");

  EXPECT_EQ(file.columns.at(0).data.at(0), "0");
  EXPECT_EQ(file.columns.at(0).data.at(1), "2");

  EXPECT_EQ(file.columns.at(1).data.at(0), "1");
  EXPECT_EQ(file.columns.at(1).data.at(1), "3");

  (void)remove("test_data.csv");
}

TEST(csv, load_with_tab)
{
  csv::file file;

  create_file("test_data.tsv", "a\tb\n0\t1\n2\t3\n");

  file.load("test_data.tsv", '\t');

  EXPECT_EQ(file.columns.at(0).name, "a");
  EXPECT_EQ(file.columns.at(1).name, "b");

  EXPECT_EQ(file.columns.at(0).data.at(0), "0");
  EXPECT_EQ(file.columns.at(0).data.at(1), "2");

  EXPECT_EQ(file.columns.at(1).data.at(0), "1");
  EXPECT_EQ(file.columns.at(1).data.at(1), "3");

  (void)remove("test_data.tsv");
}

namespace {

csv::column_type
guess_column_type(const std::vector<std::string>& values)
{
  {
    std::ofstream file("tmp.csv");
    file << "data\n";
    for (const auto& value : values)
      file << value << '\n';
  }

  csv::file file;

  file.load("tmp.csv");

  const auto column_type = file.columns.at(0).guess_column_type();

  (void)remove("tmp.csv");

  return column_type;
}

} // namespace

TEST(csv, guess_unknown_column_type)
{
  const auto type = guess_column_type({ "1.2", "45", "5e-3", "mocha", "74.32e12", "1.0e+1", "2e+1" });

  EXPECT_EQ(type, csv::column_type::unknown);
}

TEST(csv, guess_numerical_column_type)
{
  const auto type = guess_column_type({ "1.2", "45", "-8.0", "+0.4", "5e-3", "9e2", "74.32e12", "1.0e+1", "2e+1" });

  EXPECT_EQ(type, csv::column_type::numerical);
}
