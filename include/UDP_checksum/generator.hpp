#pragma once

#include <optional>
#include <cstdint>
#include <istream>

std::optional<std::uint16_t> generate_checksum(std::istream& in);