#include <cstring>
#include <map>
#include <string>
#include <stdlib.h>
#include "try.hpp"

bool try_long(const std::string &str, long &l) {
	char *p;
	l = strtol(str.c_str(), &p, 10);
	return *p == 0;
}

std::string try_date(const std::string &str) {
	static std::map<std::string, long> months = {
		{"Jan", 1},
		{"Feb", 2},
		{"Mar", 3},
		{"Apr", 4},
		{"May", 5},
		{"Jun", 6},
		{"Jul", 7},
		{"Aug", 8},
		{"Sep", 9},
		{"Oct", 10},
		{"Nov", 11},
		{"Dec", 12}
	};
	char out[16];
	long month, day, year, hour, minute;

	if (str.size() != 19 ||
			!isspace(str.at(3)) || !isspace(str.at(6)) || !isspace(str.at(11)) ||
			str.at(14) != ':' ||
			str.at(18) != 'M') {
		return "";
	}

	// Apr  9 2015 11:50PM
	auto search = months.find(str.substr(0, 3));
	if (search == months.end() ||
			!try_long(str.substr(4, 2), day) ||
			!try_long(str.substr(7, 4), year) ||
			!try_long(str.substr(12, 2), hour) ||
			!try_long(str.substr(15, 2), minute)) {
		return "";
	}

	hour %= 12;
	if (str.at(17) == 'P') {
		hour += 12;
	} else if (str.at(17) != 'A') {
		return "";
	}
	month = search->second;

	if (hour == 0 && minute == 0) {
		sprintf(out, "%04ld-%02ld-%02ld", year, month, day);
	} else {
		sprintf(out, "%04ld-%02ld-%02ld %02ld:%02ld", year, month, day, hour, minute);
	}

	return std::string(out);
}

char *commafy(const char *str) {
  size_t i, j, n, offset, is_negative;
  size_t len = strlen(str);
  static char out[30];

  is_negative = str[0] == '-';

  // Find the first decimal place
  for (i = 0; i < len; i++)
    if (str[i] == '.')
      break;

  // Offset is initially equal to the number of commas we'll need
  offset = (i - is_negative - 1) / 3;

  // Copy the fractional part
  j = i;
  if (i != len)
    for ( ; j < len; j++)
      out[j + offset] = str[j];
  out[j + offset] = '\0';

  // Copy the integer part starting from the ones digit, placing a comma after
  // every three digits
  n = 0;
  while (i-- > 0) {
    out[i + offset] = str[i];
    if (n++ % 3 == 2) {
      offset--;
      out[i + offset] = ',';
    }
  }

  return out;
}

std::string try_number(const std::string &str) {
	double d;
	char *endptr;
	char out[30];

	d = strtod(str.c_str(), &endptr);

	if (*endptr == 0) {
		sprintf(out, "%.12g", d);
		return std::string(commafy(out));
	} else {
		return "";
	}
}

std::string try_long_number(const std::string &str) {
	double d;
	char *endptr;
	char out[30];

	d = strtod(str.c_str(), &endptr);

	if (*endptr == 0) {
		sprintf(out, "%.12g", d);
		return std::string(out);
	} else {
		return "";
	}
}

// vim:ts=4 sts=4 sw=4 noet
