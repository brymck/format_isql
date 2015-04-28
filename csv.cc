#include <iostream>
#include <string>
#include <sstream>

std::string str_as_csv(const std::string &str) {
	std::size_t found = str.find_first_of("\",\n");
	if (found == std::string::npos) {
		return str;
	}
	found = 0;

	std::stringstream ss;
	std::size_t prev = 0;
	ss << '"';
	while ((found = str.find_first_of('"', found)) != std::string::npos) {
		ss << str.substr(prev, found - prev + 1) << '"';
		++found;
	}

	ss << '"';
	return ss.str();
}

// vim:ts=4 sts=4 sw=4 noet
