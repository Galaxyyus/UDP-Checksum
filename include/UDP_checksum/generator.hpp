#pragma once

#include <optional>
#include <cstdint>
#include <istream>
#include <vector>
#include <iostream>

std::optional<std::uint16_t> generate_checksum(std::istream& in);