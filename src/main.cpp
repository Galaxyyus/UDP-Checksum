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

#include <UDP_checksum/generator.hpp>

void show_help() {
	// TODO: please replace the placeholder in the instructions file :)
	std::istringstream file(R"(Usage Instructions

		This program generates or checks a UDP checksum.

		Examples:
	main.exe - g - f input.txt
		main.exe --generate "hello\nworld"
		main.exe - c - f input.txt
		main.exe --check "hello\nworld"

		Options :
		-g, --generate   Generate and display the checksum
		- c, --check      Check or verify the checksum
		- f, --file       Read input from a file

		You can provide either :
	-a file using - f or --file
		- or a single inline input string(supports escape characters)

		Notes :
		-Do not provide both a file and an inline input string at the same time.
		- If no arguments are provided, this message is shown.)");

	char c;
	srand(time(0));

	int speed = 40; // millis per char
	int variation = 20;

	int chars = 1 + rand() % 7;
	int wait = (speed - variation + 2 * (rand() % variation));

	//this ghostly typing makes sure the user reads the instructions fully
	while (file.get(c)) {
		if (chars <= 0) {
			chars = 1 + rand() % 7;
			wait = wait = (speed - variation + 2 * (rand() % variation));
		}
		std::cout << c;
		std::cout.flush();
		std::this_thread::sleep_for(std::chrono::milliseconds(wait));
		chars--;
	}
}

int main(const int argc, const char* argv[]) {

	// checking if an argument is already there
	if (argc < 2) {
		show_help();
		return 1;
	}

	const std::string option = argv[1];
	if (option != "-g" && option != "--generate" && option != "-c" && option != "--check") {
		std::cout << "Unknown option: " << option << " !\n";
		return 1;
	}

	const bool is_generate = (option == "-g" || option == "--generate");
	const bool is_check = (option == "-c" || option == "--check");

	std::string inline_input, input_filename;

	for (int i = 2; i < argc; i++) {
		const std::string arg = argv[i];

		// Inline input
		if (arg[0] != '-') {
			if (inline_input.empty()) {
				inline_input = arg;
				continue;
			} else {
				std::cout << "Error: Only one inline input can be provided!\n";
				return 1;
			}
		}
		// Filename argument
		else if (arg == "-f" || arg == "--file") {
			if (i + 1 == argc) {
				std::cout << "Error: No filename provided after -f or --file flag!\n";
				return 1;
			} else {
				input_filename = argv[++i];
			}
		}
		// Invalid flags
		else {
			std::cout << "Unknown flag: " << arg << " !\n";
			return 1;
		}
	}

	// Both or neither input file and inline input provided
	if (!inline_input.empty() && !input_filename.empty() && is_generate) {
		std::cout << "Error: cannot use both -f and an inline input string with -g!\n";
		return 1;
	}
	if (inline_input.empty() && input_filename.empty()) {
		std::cout << "Error: no input provided. Use -f <path> or pass a string directly!\n";
		return 1;
	}

	// Handle checksum generation
	std::optional<std::uint16_t> checksum;
	if (!input_filename.empty()) {
		std::ifstream input(input_filename, std::ios::binary);
		if (!input) {
			std::cerr << "Error: could not open file: " << input_filename << '\n';
			return 1;
		}

		checksum = generate_checksum(input);
	}
	if (!inline_input.empty()) {
		std::istringstream input(inline_input);
		checksum = generate_checksum(input);
	}

	if (!checksum) {
		std::cout << "Aborting, operation failed!!!\n";
		return 1;
	}

	if (is_generate) {
		std::cout << std::bitset<16>(*checksum) << '\n';
		return 0;
	}
	if (is_check) {
		std::cout << "Please enter the checksum to check against (in binary): ";
		std::string user_checksum_str;
		std::cin >> user_checksum_str;
		if (user_checksum_str.length() != 16) {
			std::cout << "Error: Invalid checksum format. Please provide a 16-bit binary string.\n";
			return 1;
		} else {
			std::uint16_t user_checksum = 0;
			for (char c : user_checksum_str) {
				if (c != '0' && c != '1') {
					std::cout << "Error: Invalid checksum format. Please provide a 16-bit binary string.\n";
					return 1;
				}
				user_checksum = (user_checksum << 1) | (c - '0');
			}
			if (user_checksum == *checksum) {
				std::cout << "Checksum is valid.\n";
			} else {
				std::cout << "Checksum is invalid.\n";
				std::cout << "Expected: " << std::bitset<16>(*checksum) << ", but got: " << user_checksum_str << '\n';
			}
		}

		return 0;
	}
}
