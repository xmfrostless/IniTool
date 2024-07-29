#include "IniTool.h"
#include <regex>

std::shared_ptr<IniObject> IniTool::Parse(const std::string& src) const {
	auto ret = std::shared_ptr<IniObject>(new IniObject());
	std::size_t pos = 0;
	std::string section = "";
	while (pos <= src.size()) {
		MatchSection(src, &pos, &section);
		std::string key;
		if (!MatchNameString(src, &pos, &key)) {
			break;
		}
		std::string value;
		if (!MatchValue(src, &pos, &value)) {
			break;
		}
		ret->Set(section, key, value);
	}
	return ret;
}

std::string IniTool::ToString(std::shared_ptr<IniObject> iniObject) const {
	const auto& data = iniObject->Data();
	std::string ret = "";
	for (auto [sectionKey, sectionValue] : data) {
		ret += "[" + sectionKey + "]\n";
		for (auto [itemKey, itemValue] : sectionValue) {
			std::regex longDoubleRegex(R"(^[-+]?(?:[0-9]+)?(?:\.[0-9]+)?(?:[eE][-+]?[0-9]+)?$)");
			bool notString = std::regex_match(itemValue, longDoubleRegex) || IsBool(itemValue);
			if (notString) {
				ret += itemKey + "=" + itemValue + "\n";
			} else {
				ret += itemKey + "=\"" + itemValue + "\"\n";
			}
		}
	}
	return ret;
}

bool IniTool::MatchSection(const std::string& src, std::size_t* pos, std::string* section) const {
	std::size_t tmp_pos = *pos;

	MatchSpace(src, true, &tmp_pos);
	if (tmp_pos >= src.size()) {
		return false;
	}
	if (src[tmp_pos] != '[') {
		return false;
	}
	++tmp_pos;
	MatchSpace(src, true, &tmp_pos);
	if (tmp_pos >= src.size()) {
		return false;
	}
	std::string tmp_name;
	if (!MatchNameString(src, &tmp_pos, &tmp_name)) {
		return false;
	}
	MatchSpace(src, true, &tmp_pos);
	if (tmp_pos >= src.size()) {
		return false;
	}
	if (src[tmp_pos] != ']') {
		return false;
	}
	*pos = tmp_pos + 1;
	*section = tmp_name;
	return true;
}

bool IniTool::MatchValue(const std::string& src, std::size_t* pos, std::string* value) const {
	std::size_t tmp_pos = *pos;
	MatchSpace(src, false, &tmp_pos);
	if (tmp_pos >= src.size()) {
		return false;
	}
	if (src[tmp_pos] != '=') {
		return false;
	}
	++tmp_pos;
	MatchSpace(src, false, &tmp_pos);
	if (src[tmp_pos] == '\"') {
		++tmp_pos;
	}
	std::size_t end_offset = 0;
	std::size_t mark_pos = tmp_pos;
	while (tmp_pos < src.size()) {
		char ch = src[tmp_pos];
		if (ch == ';' || ch == '#' || ch == '\n' || ch == '\r') {
			break;
		} else if (ch == '/') {
			std::size_t next_pos = tmp_pos + 1;
			if (next_pos < src.size() && src[next_pos] == '/') {
				break;
			}
		} else if (ch == '\"') {
			end_offset = 1;
			break;
		}
		++tmp_pos;
	}
	std::size_t end_pos = tmp_pos + end_offset;
	std::size_t tmp_check_pos = tmp_pos - 1;
	while (tmp_check_pos >= 0 && src[tmp_check_pos] == ' ') {
		--tmp_check_pos;
		--tmp_pos;
	}
	if (tmp_pos <= mark_pos) {
		*pos = end_pos;
		*value = "";
		return true;
	}

	*pos = end_pos;
	*value = src.substr(mark_pos, tmp_pos - mark_pos);
	return true;
}

bool IniTool::MatchNameString(const std::string& src, std::size_t* pos, std::string* name) const {
	std::size_t tmp_pos = *pos;
	MatchSpace(src, true, &tmp_pos);
	std::size_t mark_pos = tmp_pos;
	while (tmp_pos < src.size()) {
		char ch = src[tmp_pos];
		if (ch >= 'a' && ch <= 'z') {
		} else if (ch >= 'A' && ch <= 'Z') {
		} else if (ch >= '0' && ch <= '9') {
		} else if (ch == '-' || ch == '.' || ch == '_' || ch == '$' || ch == '@' || ch == ' ') {
		} else {
			break;
		}
		++tmp_pos;
	}
	if (tmp_pos == mark_pos) {
		return false;
	}
	std::size_t end_pos = tmp_pos;
	std::size_t tmp_check_pos = tmp_pos - 1;
	while (tmp_check_pos >= 0 && src[tmp_check_pos] == ' ') {
		--tmp_check_pos;
		--tmp_pos;
	}
	if (tmp_pos <= mark_pos) {
		return false;
	}

	*pos = end_pos;
	*name = src.substr(mark_pos, tmp_pos - mark_pos);
	return true;
}

void IniTool::MatchSpace(const std::string& src, bool checkLineEnd, std::size_t* pos) const {
	while (*pos < src.size()) {
		char ch = src[*pos];
		if (ch == ';' || ch == '#') {
			*pos = src.find('\n', *pos);
			if (*pos != std::string::npos) {
				++(*pos);
			}
		} else if (ch == '/') {
			std::size_t next_pos = *pos + 1;
			if (next_pos < src.size() && src[next_pos] == '/') {
				*pos = src.find('\n', next_pos);
				if (*pos != std::string::npos) {
					++(*pos);
				}
			}
		} else if (ch == ' ' || ch == '\t' || ch == '\r') {
			++(*pos);
		} else if (checkLineEnd && ch == '\n') {
			++(*pos);
		} else {
			break;
		}
	}
}

bool IniTool::IsBool(const std::string& src) const {
	std::size_t size = src.size();
	if (size == 4) {
		return	(src[0] == 'T' || src[0] == 't') &&
				(src[1] == 'R' || src[1] == 'r') &&
				(src[2] == 'U' || src[2] == 'u') &&
				(src[3] == 'E' || src[3] == 'e');
	} else if (size == 5) {
		return	(src[0] == 'F' || src[0] == 'f') &&
				(src[1] == 'A' || src[1] == 'a') &&
				(src[2] == 'L' || src[2] == 'l') &&
				(src[3] == 'S' || src[3] == 's') &&
				(src[4] == 'E' || src[4] == 'e');
	}
	return false;
}
