#include "exception.hpp"

file_open_exception::file_open_exception(const char* what, std::string path)
  : std::runtime_error(what)
    , path_(std::move(path))
{
}
