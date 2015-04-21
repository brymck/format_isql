#include <iostream>
#include <map>
#include <stdlib.h>
#include <string>
#include <vector>
#include "try.hpp"

using ::std::string;
using ::std::vector;
using ::std::cout;
using ::std::endl;

enum FieldType {
	UNKNOWN,
	NUMBER,
	DATETIME,
	STRING
};

struct Datum {
	string str;
	string alt;
};

struct Format {
	size_t header;
	size_t length;
	size_t left;
	size_t right;
	size_t alt;
	size_t longest;
	FieldType type;
};

string trim(const string &str) {
	size_t begin = str.find_first_not_of(" ");
	size_t end = str.find_last_not_of(" ");
	
	if (begin != string::npos && end != string::npos) {
		return str.substr(begin, end - begin + 1);
	} else {
		return str;
	}
}

vector<string> split_at_indices(const string &line, vector<size_t> offsets) {
	vector<string> substrings;
	size_t prev = 1;

	for (auto it = offsets.begin(); it != offsets.end(); ++it) {
		substrings.push_back(trim(line.substr(prev, *it - prev)));
		prev = *it + 1;
	}

	return substrings;
}

vector<size_t> get_offsets(const string &line) {
	vector<size_t> offsets;

	for (auto it = std::next(line.begin()); it != line.end(); ++it) {
		if (isspace(*it)) {
			offsets.push_back(it - line.begin());
		}
	}

	return offsets;
}

bool is_data_line(const string &line) {
	return isspace(line[0]) && line.size() > 1;
}

string next_data_line() {
	string line;
	while (getline(std::cin, line)) {
		if (is_data_line(line)) {
			return line;
		}
	}
	return "";
}

void print_headers(const vector<string> &headers, const vector<Format> &formats) {
	for (auto h = headers.begin(), f = formats.begin(); h != headers.end(); ++h, ++f) {
		std::cout << *h << string(f->longest - h->size(), ' ') << ' ';
	}
	std::cout << std::endl;
}

void print_separators(const vector<Format> &formats) {
	for (auto f = formats.begin(); f != formats.end(); ++f) {
		std::cout << string(f->longest, '-') << ' ';
	}
	std::cout << std::endl;
}

inline size_t first_pos_or_length(const string &str, const char delim) {
	size_t pos = str.find_first_of(delim);
	if (pos == string::npos) {
		return str.size();
	} else {
		return pos;
	}
}

void print_data(const Datum &datum, const Format &format) {
	string s;
	size_t left = 0;
	size_t right = 0;
	size_t midpoint;

	switch (format.type) {
		case STRING:
			s = datum.str;
			right = format.longest - s.size();
			break;
		case NUMBER:
			s = datum.alt;
			midpoint = first_pos_or_length(s, '.');
			right = (format.right == 0 ? 0 : format.right - (s.size() - midpoint));
			left = format.longest - s.size() - right;
			break;
		case DATETIME:
			s = datum.alt;
			midpoint = first_pos_or_length(s, ' ');
			right = (format.right == 0 ? 0 : format.right - (s.size() - midpoint));
			left = format.longest - s.size() - right;
			break;
		default:
			break;
	}

	std::cout << string(left, ' ') << s << string(right, ' ') << ' ';
}

inline void set_if_greater(size_t &ref, size_t value) {
	if (value > ref) {
		ref = value;
	}
}

void print_all(
		vector<string> headers,
		vector<vector<Datum>> data,
		vector<Format> formats
		) {
	print_headers(headers, formats);
	print_separators(formats);
	for (auto row = data.begin(); row != data.end(); ++row) {
		for (auto d = row->begin(), f = formats.begin(); d != row->end(); ++d, ++f) {
			print_data(*d, *f);
		}
		std::cout << std::endl;
	}
}

bool try_convert(
		Format &format,
		Datum &datum,
		const string &str,
		FieldType field_type,
		const char delim,
		string (*convert)(const string&)) {
	if (format.type == field_type || format.type == UNKNOWN) {
		string value = convert(str);
		if (value.empty()) {
			if (format.type == field_type) {
				format.type = STRING;
				return 1;
			}
		} else {
			datum.alt = value;
			size_t size = value.size();
			size_t middle = value.find_first_of(delim);
			if (middle == string::npos) {
				set_if_greater(format.left, size);
			} else {
				set_if_greater(format.left, middle - 1);
				set_if_greater(format.right, size - middle);
			}
			format.type = field_type;
			return 1;
		}
	}

	return 0;
}

int main() {
	string header = next_data_line();
	string separators = next_data_line();
	string line;

	vector<size_t> offsets = get_offsets(separators);
	vector<Format> formats;
	vector<vector<Datum>> data;
	vector<string> header_parts = split_at_indices(header, offsets);

	for (auto h = header_parts.begin(); h != header_parts.end(); ++h) {
		formats.push_back(Format{ h->size(), 0, 0, 0, 0, 0, UNKNOWN });
	}

	while (getline(std::cin, line)) {
		if (is_data_line(line)) {
			vector<Datum> row;

			vector<string> parts = split_at_indices(line, offsets);
			for (auto it = parts.begin(), f = formats.begin(); it != parts.end(); ++it, ++f) {
				Datum datum = Datum{*it, ""};
				set_if_greater(f->length, it->size());

				if (f->type != STRING &&
						!try_convert(*f, datum, *it, DATETIME, ' ', &try_date) &&
						!try_convert(*f, datum, *it, NUMBER, '.', &try_number)) {
					f->type = STRING;
				}

				row.push_back(datum);
			}

			data.push_back(row);
		}
	}

	for (auto f = formats.begin(); f != formats.end(); ++f) {
		if (f->type != STRING) {
			f->length = f->left + f->right + (f->right != 0 ? 1 : 0);
		}
		f->longest = std::max(f->length, f->header);
	}

	print_all(header_parts, data, formats);

	return 0;
}

// vim:ts=4 sts=4 sw=4 noet
