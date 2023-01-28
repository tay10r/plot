#include <gtest/gtest.h>

#include "csv.hpp"
#include "data_set.hpp"

#include <fstream>

namespace {

csv_data_set
from_csv(const std::string& data)
{
  {
    std::ofstream file("tmp.csv");
    file << data;
  }

  csv::file file;

  file.load("tmp.csv");

  (void)std::remove("tmp.csv");

  return csv_data_set(file);
}

} // namespace

TEST(data_set, load_from_csv)
{
  const auto ds = from_csv("a,b,c\n0,&,2\n3,*,5\n");
  EXPECT_EQ(ds.get_column_count(), 2);
  EXPECT_EQ(std::string(ds.get_column_name(0)), "a");
  EXPECT_EQ(std::string(ds.get_column_name(1)), "c");

  const auto& indices = ds.get_indices();
  EXPECT_EQ(indices.size(), 2);
  EXPECT_EQ(indices.at(0), 0);
  EXPECT_EQ(indices.at(1), 1);

  const auto& data = ds.get_data();
  EXPECT_NEAR(data.at(0, 0), 0.0f, 0.001f);
  EXPECT_NEAR(data.at(0, 1), 2.0f, 0.001f);
  EXPECT_NEAR(data.at(1, 0), 3.0f, 0.001f);
  EXPECT_NEAR(data.at(1, 1), 5.0f, 0.001f);
}
