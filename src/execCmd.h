#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <string>

int exec(const std::string &cmd, std::string &out) {
	std::array<char, 128> buffer;
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe) throw std::runtime_error("popen() failed!");
	while (!feof(pipe)) {
		if (fgets(buffer.data(), 128, pipe) != nullptr)
			out += buffer.data();
	}
	int res = pclose(pipe);
	return WEXITSTATUS(res);
}
