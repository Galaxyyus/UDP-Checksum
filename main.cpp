#include <iostream>
#include <fstream>
#include <sstream>
#include <istream>
#include <cstdint>
#include <vector>
#include <optional>
#include <bitset>
#include <fstream>
#include <thread>
#include <chrono>
#include <ctime>

std::optional<std::uint16_t> generate_checksum(std::istream &in)
{
	const size_t CHUNK_SIZE = 64 * 1024 * 1024;
	std::vector<std::uint8_t> buffer(CHUNK_SIZE);

	std::uint32_t checksum = 0;//why 32???

	std::uint8_t leftover_byte = 0;
	size_t leftover_count = 0;

	while (true)
	{
		if (leftover_count > 0)
			buffer[0] = leftover_byte;

		in.read(reinterpret_cast<char *>(buffer.data() + leftover_count), CHUNK_SIZE - leftover_count);

		// Error checks
		if (in.bad())
		{
			std::cerr << "Error: input stream encountered an I/O error!\n";
			return std::nullopt;
		}
		if (in.fail() && !in.eof())
		{
			std::cerr << "Error: input stream encounter an unknown error!\n";
			return std::nullopt;
		}

		auto bytes_read = in.gcount();
		if (bytes_read == 0)
			break; // Reached EOF
		auto total_bytes = bytes_read + leftover_count;
		leftover_count = 0;

		size_t i = 0;
		while (i + 1 < total_bytes)
		{
			std::uint8_t a = buffer[i++];
			std::uint8_t b = buffer[i++];

			std::uint16_t octet_pair = (a << 8) | b;

			checksum += octet_pair;
		}

		if (i < total_bytes)
		{
			leftover_byte = buffer[i];
			leftover_count = 1;
		}
	}

	if (leftover_count > 0)
	{
		std::uint16_t octet_pair = leftover_byte << 8;
		checksum += octet_pair;
	}

	while (checksum >> 16)
	{
		checksum = (checksum & 0xFFFF) + (checksum >> 16);
	}

	if (checksum == 0xFFFF)
		return checksum;
	else
		return ~checksum;
}

void show_help(){
	// TODO: please replace the placeholder in the instructions file :)
		std::ifstream file("instructions.txt");
		char c;
		srand(time(0));

		int speed = 125; // millis per char

		int chars = 1 + rand() % 10;
		int wait = (1 + rand() % speed);

		while (file.get(c))
		{
			if (chars <= 0)
			{
				chars = 1 + rand() % 10;
				wait = (1 + rand() % speed);
			}
			std::cout << c;
			std::cout.flush();
			std::this_thread::sleep_for(std::chrono::milliseconds(wait));
			chars--;
		}
}

int main(const int argc, const char *argv[])
{

	// checking if an argument is already there
	if (argc < 2)
	{
		show_help();
		return 1;
	}

	const std::string option = argv[1];
	if (option != "-g" && option != "--generate" && option != "-c" && option != "--check")
	{
		std::cout << "Unknown option: " << option << " !\n";
		return 1;
	}

	const bool is_generate = (option == "-g" || option == "--generate");

	std::string inline_input, input_filename;

	for (int i = 2; i < argc; i++)
	{
		const std::string arg = argv[i];

		// Inline input
		if (arg[0] != '-')
		{
			if (inline_input.empty())
			{
				inline_input = arg;
				continue;
			}
			else
			{
				std::cout << "Error: Only one inline input can be provided!\n";
				return 1;
			}
		}
		// Filename argument
		else if (arg == "-f" || arg == "--file")
		{
			if (i + 1 == argc)
			{
				std::cout << "Error: No filename provided after -f or --file flag!\n";
				return 1;
			}
			else
			{
				input_filename = argv[++i];
			}
		}
		// Invalid flags
		else
		{
			std::cout << "Unknown flag: " << arg << " !\n";
			return 1;
		}
	}

	// Both or neither input file and inline input provided
	if (!inline_input.empty() && !input_filename.empty() && is_generate)
	{
		std::cout << "Error: cannot use both -f and an inline input string with -g!\n";
		return 1;
	}
	if (inline_input.empty() && input_filename.empty())
	{
		std::cout << "Error: no input provided. Use -f <path> or pass a string directly!\n";
		return 1;
	}

	// Handle checksum generation
	std::optional<std::uint16_t> checksum;
	if (!input_filename.empty())
	{
		std::ifstream input(input_filename, std::ios::binary);
		if (!input)
		{
			std::cerr << "Error: could not open file: " << input_filename << '\n';
			return 1;
		}

		checksum = generate_checksum(input);
	}
	if (!inline_input.empty())
	{
		std::istringstream input(inline_input);
		checksum = generate_checksum(input);
	}

	if (!checksum)
	{
		std::cout << "Aborting, operation failed!!!\n";
		return 1;
	}

	if (is_generate)
	{
		std::cout << std::bitset<16>(*checksum) << '\n';
		return 0;
	}
}
