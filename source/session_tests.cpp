#include <gtest/gtest.h>

#include "session.hpp"

namespace {

class null_data_set final : public data_set
{
public:
  const char* get_name() const override
  {
    return "";
  }

  void accept(data_set_visitor&) const override
  {
  }
};

} // namespace

TEST(session, close_data_set)
{
  session s;

  auto a = std::make_unique<null_data_set>();
  auto b = std::make_unique<null_data_set>();
  auto c = std::make_unique<null_data_set>();

  auto a_ptr = a.get();
  auto b_ptr = b.get();
  auto c_ptr = c.get();

  s.add_data_set(std::move(a));
  s.add_data_set(std::move(b));
  s.add_data_set(std::move(c));

  EXPECT_EQ(s.get_data_set_count(), 3);

  EXPECT_EQ(s.get_data_set(0), a_ptr);
  EXPECT_EQ(s.get_data_set(1), b_ptr);
  EXPECT_EQ(s.get_data_set(2), c_ptr);

  s.close_data_set(b_ptr);

  EXPECT_EQ(s.get_data_set(0), a_ptr);
  EXPECT_EQ(s.get_data_set(1), c_ptr);
}
