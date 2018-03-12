#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <string>

int exec(const std::string &cmd, std::string &out) {
	std::array<char, 128> buffer;
	std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"));
	if (!pipe) throw std::runtime_error("popen() failed!");
	while (!feof(pipe.get())) {
		if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
			out += buffer.data();
	}
	int res = WEXITSTATUS(pclose(pipe.get()));
	return res;
}
