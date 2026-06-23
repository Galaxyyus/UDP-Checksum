#include <optional>
#include <cstdint>
#include <istream>
#include <vector>
#include <iostream>

#include <UDP_checksum/generator.hpp>

std::optional<std::uint16_t> generate_checksum(std::istream& in) {
	const int chunk_size_MB = 64;
	const size_t CHUNK_SIZE = chunk_size_MB * 1024 * 1024;
	std::vector<std::uint8_t> buffer(CHUNK_SIZE);

	std::uint32_t checksum = 0;//why 32???

	std::uint8_t leftover_byte = 0;
	size_t leftover_count = 0;

	while (true) {
		if (leftover_count > 0)
			buffer[0] = leftover_byte;

		in.read(reinterpret_cast<char*>(buffer.data() + leftover_count), CHUNK_SIZE - leftover_count);

		// Error checks
		if (in.bad()) {
			std::cerr << "Error: input stream encountered an I/O error!\n";
			return std::nullopt;
		}
		if (in.fail() && !in.eof()) {
			std::cerr << "Error: input stream encounter an unknown error!\n";
			return std::nullopt;
		}

		auto bytes_read = in.gcount();
		if (bytes_read == 0)
			break; // Reached EOF
		auto total_bytes = bytes_read + leftover_count;
		leftover_count = 0;

		size_t i = 0;
		while (i + 1 < total_bytes) {
			std::uint8_t a = buffer[i++];
			std::uint8_t b = buffer[i++];

			std::uint16_t octet_pair = (a << 8) | b;

			checksum += octet_pair;
		}

		if (i < total_bytes) {
			leftover_byte = buffer[i];
			leftover_count = 1;
		}
	}

	if (leftover_count > 0) {
		std::uint16_t octet_pair = leftover_byte << 8;
		checksum += octet_pair;
	}

	while (checksum >> 16) {
		checksum = (checksum & 0xFFFF) + (checksum >> 16);
	}

	if (checksum == 0xFFFF)
		return checksum;
	else
		return ~checksum;
}