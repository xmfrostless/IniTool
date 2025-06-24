#include "IniTool.h"
#include <regex>

IniObject IniTool::Parse(const std::string& src) {
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> data;
	std::size_t pos = 0;
	std::string section = "";
	while (pos <= src.size()) {
		MatchSection(src, &pos, &section);
		std::string key;
		if (!MatchKey(src, &pos, &key)) {
			break;
		}
		if (!MatchAssign(src, &pos)) {
			break;
		}
		std::string value;
		if (!MatchValue(src, &pos, &value)) {
			break;
		}
		data[section][key] = std::move(value);
	}
	return IniObject(std::move(data));
}

std::string IniTool::Stringify(const IniObject& iniObject) {
	const auto& data = iniObject.Data();
	std::string ret = "";
	for (const auto& sectionItem : data) {
		ret += "[" + sectionItem.first + "]\n";
		for (const auto& valueItem : sectionItem.second) {
			std::regex longDoubleRegex(R"(^[-+]?(?:[0-9]+)?(?:\.[0-9]+)?(?:[eE][-+]?[0-9]+)?$)");
			bool notString = std::regex_match(valueItem.second, longDoubleRegex) || IsBool(valueItem.second);
			if (notString) {
				ret += valueItem.first + "=" + valueItem.second + "\n";
			} else {
				ret += valueItem.first + "=\"" + valueItem.second + "\"\n";
			}
		}
	}
	return ret;
}

bool IniTool::MatchSection(const std::string& src, std::size_t* pos, std::string* section) {
	std::size_t p = *pos;
	MatchSpaceAndComment(src, &p);
	if (!MatchWord(src, "[", &p)) {
		return false;
	}
	MatchSpaceAndComment(src, &p);
	std::string str;
	if (!MatchKey(src, &p, &str)) {
		return false;
	}
	MatchSpaceAndComment(src, &p);
	if (!MatchWord(src, "]", &p)) {
		return false;
	}
	*pos = p;
	*section = std::move(str);
	return true;
}

bool IniTool::MatchKey(const std::string& src, std::size_t* pos, std::string* name) {
	MatchSpaceAndComment(src, pos);
	auto p = *pos;
	if (MatchString(src, &p, name)) {
		*pos = p;
		return true;
	}
	std::vector<char> buffers;
	while (p < src.size()) {
		char ch = src[p];
		if ((ch >= 'a' && ch <= 'z') ||
			(ch >= 'A' && ch <= 'Z') ||
			(ch >= '0' && ch <= '9') ||
			(ch == '_' || ch == '-' || ch == '.')) {
			buffers.emplace_back(ch);
			++p;
		} else {
			break;
		}
	}
	if (buffers.empty()) {
		return false;
	}
	char begin = *buffers.begin();
	char end = *buffers.rbegin();
	if (begin == '-' || begin == '.' || end == '-' || end == '.') {
		return false;
	}
	*pos = p;
	name->assign(buffers.begin(), buffers.end());
	return true;
}

bool IniTool::MatchValue(const std::string& src, std::size_t* pos, std::string* value) {
	MatchSpaceAndComment(src, pos);
	auto p = *pos;
	if (MatchString(src, &p, value)) {
		*pos = p;
		return true;
	}
	auto head = p;
	while (p < src.size()) {
		char ch = src[p];
		if (ch == '\n' || ch == '\r' ||
			ch == ';' || ch == '#' ||
			(ch == '/' && p + 1 < src.size() && src[p + 1] == '/')) {
			break;
		}
		++p;
	}

	*pos = p;

	while (p > head) {
		char ch = src[p - 1];
		if (ch == ' ' || ch == '\t') {
			--p;
		} else {
			break;
		}
	}
	if (p == head) {
		return false;
	}
	*value = std::move(src.substr(head, p - head));
	return true;
}

bool IniTool::MatchAssign(const std::string& src, std::size_t* pos) {
	MatchSpaceAndComment(src, pos);
	return MatchWord(src, "=", pos)
		|| MatchWord(src, ":", pos);
}

void IniTool::MatchSpaceAndComment(const std::string& src, std::size_t* pos) {
	while (*pos < src.size()) {
		char ch = src[*pos];
		if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
			++(*pos);
		} else if (ch == ';' || ch == '#' || MatchWord(src, "//", pos)) {
			*pos = src.find('\n', *pos);
			if (*pos != std::string::npos) {
				++(*pos);
			}
		} else {
			break;
		}
	}
}

bool IniTool::IsBool(const std::string& src) const {
	std::size_t size = src.size();
	if (size == 4) {
		return (src[0] == 'T' || src[0] == 't') &&
			(src[1] == 'R' || src[1] == 'r') &&
			(src[2] == 'U' || src[2] == 'u') &&
			(src[3] == 'E' || src[3] == 'e');
	} else if (size == 5) {
		return (src[0] == 'F' || src[0] == 'f') &&
			(src[1] == 'A' || src[1] == 'a') &&
			(src[2] == 'L' || src[2] == 'l') &&
			(src[3] == 'S' || src[3] == 's') &&
			(src[4] == 'E' || src[4] == 'e');
	}
	return false;
}

bool IniTool::MatchPair(const std::string& src, const std::string& left, const std::string& right, std::size_t* pos, std::string* result) {
	auto p = *pos;
	if (!MatchWord(src, left, &p)) {
		return false;
	}
	std::vector<char> buffers;
	bool change = false;
	while (p < src.size()) {
		if (MatchWord(src, right, &p)) {
			result->assign(buffers.begin(), buffers.end());
			*pos = p;
			return true;
		}
		char ch = src[p++];
		if (!change && ch == '\\') {
			change = true;
			continue;
		}
		if (change) {
			if (ch == '\\') {
				buffers.emplace_back('\\');
			} else if (ch == 'n') {
				buffers.emplace_back('\n');
			} else if (ch == 't') {
				buffers.emplace_back('\t');
			} else if (ch == 'b') {
				if (!buffers.empty()) {
					buffers.pop_back();
				}
			} else if (ch == 'r') {
				buffers.emplace_back('\r');
			} else if (ch == '"') {
				buffers.emplace_back('\"');
			} else if (ch == '\'') {
				buffers.emplace_back('\'');
			} else {
				buffers.emplace_back(ch);
			}
			change = false;
		} else {
			buffers.emplace_back(ch);
		}
	}
	return false;
}

bool IniTool::MatchString(const std::string& src, std::size_t* pos, std::string* result) {
	return MatchPair(src, "\"", "\"", pos, result)
		|| MatchPair(src, "'", "'", pos, result)
		|| MatchPair(src, "`", "`", pos, result);
}

bool IniTool::MatchWord(const std::string& src, const std::string& word, std::size_t* pos) {
	auto p = *pos;
	auto i = 0u;
	while (p < src.size() && i < word.size()) {
		if (src[p++] != word[i++]) {
			return false;
		}
	}
	*pos = p;
	return true;
}
