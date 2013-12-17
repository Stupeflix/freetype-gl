
#pragma once

#include <sstream>

/**
 * Convert a value from any basic type to another using
 * standard stream.
 */
template<typename To, typename From>
To convert(From const &from) {
  std::stringstream s;
  To to;
  s << from;
  s >> to;
  return to;
}
