#include "IniTool.h"
#include <vector>

IniObject INI::Parse(const std::string& src) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> data;
    std::size_t pos = 0;
    std::string section = "";
    while (pos < src.size()) {
        while (pos < src.size() && MatchSection(src, &pos, &section)) {
            if (data.find(section) == data.end()) {
                data.emplace(section, std::unordered_map<std::string, std::string>());
            }
        }
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

std::string INI::Stringify(const IniObject& iniObject) {
    const auto& data = iniObject.Data();
    std::string ret = "";
    for (const auto& sectionItem : data) {
        if (!sectionItem.first.empty()) {
            ret += "[" + sectionItem.first + "]\n";
        }
        for (const auto& valueItem : sectionItem.second) {
            if (IsNormalizeKey(valueItem.first)) {
                ret += valueItem.first;
            } else {
                ret += "\"" + valueItem.first + "\"";
            }

            if (IsNotString(valueItem.second)) {
                ret += "=" + valueItem.second + "\n";
            } else {
                std::vector<char> buffers;
                for (char ch : valueItem.second) {
                    if (NeedEscape(ch)) {
                        buffers.emplace_back('\\');
                    }
                    buffers.emplace_back(ch);
                }
                ret += "=\"" + std::string(buffers.begin(), buffers.end()) + "\"\n";
            }
        }
    }
    return ret;
}

bool INI::MatchSection(const std::string& src, std::size_t* pos, std::string* section) {
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

bool INI::MatchKey(const std::string& src, std::size_t* pos, std::string* name) {
    MatchSpaceAndComment(src, pos);
    auto p = *pos;
    if (MatchString(src, &p, name)) {
        *pos = p;
        return true;
    }
    std::vector<char> buffers;
    while (p < src.size()) {
        char ch = src[p];
        if (IsNormalizeKeyChar(ch) || ch == '-' || ch == '.') {
            buffers.emplace_back(ch);
            ++p;
        } else {
            break;
        }
    }
    if (buffers.empty()) {
        return false;
    }
    if (!IsNormalizeKeyChar(buffers[0]) || !IsNormalizeKeyChar(buffers[buffers.size() - 1])) {
        return false;
    }
    *pos = p;
    name->assign(buffers.begin(), buffers.end());
    return true;
}

bool INI::MatchValue(const std::string& src, std::size_t* pos, std::string* value) {
    auto p = *pos;
    while (p < src.size()) {
        if (src[p] == ' ' || src[p] == '\t') {
            ++p;
        } else {
            break;
        }
    }
    if (MatchString(src, &p, value)) {
        *pos = p;
        return true;
    }
    std::size_t ts = 0u;
    if (MatchParagraphToTerminator(src, { "\n", "\r", ";", "#", "\"", "\'" }, &p, &ts, value)) {
        *pos = p;
        return true;
    }
    return false;
}

bool INI::MatchAssign(const std::string& src, std::size_t* pos) {
    MatchSpaceAndComment(src, pos);
    return MatchWord(src, "=", pos)
        || MatchWord(src, ":", pos);
}

void INI::MatchSpaceAndComment(const std::string& src, std::size_t* pos) {
    while (*pos < src.size()) {
        char ch = src[*pos];
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            ++(*pos);
        } else if (ch == ';' || ch == '#') {
            *pos = src.find('\n', *pos);
            if (*pos != std::string::npos) {
                ++(*pos);
            }
        } else {
            break;
        }
    }
}

bool INI::IsNormalizeKey(const std::string& key) {
    if (key.empty()) {
        return false;
    }
    if (!IsNormalizeKeyChar(key[0])) {
        return false;
    }
    if (!IsNormalizeKeyChar(key[key.size() - 1])) {
        return false;
    }
    for (char ch : key) {
        if (!IsNormalizeKeyChar(ch) && ch != '.' && ch != '-') {
            return false;
        }
    }
    return true;
}
bool INI::IsNormalizeKeyChar(char ch) {
    return (ch >= 'a' && ch <= 'z')
        || (ch >= 'A' && ch <= 'Z')
        || (ch >= '0' && ch <= '9')
        || ch == '_';
}

bool INI::IsNotString(const std::string& value) {
    return IniObject::IsNumber(value) || IniObject::IsBool(value);
}
bool INI::NeedEscape(char ch) {
    return ch == '\\' || ch == '"' || ch == '\'' || ch == ';' || ch == '#' || ch == '`';
}

bool INI::MatchParagraphToTerminator(const std::string& src, const std::initializer_list<std::string>& terminator, std::size_t* pos, std::size_t* terminatroSize, std::string* result) {
    auto p = *pos;
    std::vector<char> buffers;
    bool change = false;
    while (p < src.size()) {
        if (src[p] == '\0') {
            p = src.size();
            break;
        }
        if (!change) {
            for (const auto& t : terminator) {
                if (MatchWord(src, t, &p)) {
                    result->assign(buffers.begin(), buffers.end());
                    *pos = p - t.size();
                    *terminatroSize = t.size();
                    return true;
                }
            }
        }
        char ch = src[p++];
        if (!change && ch == '\\') {
            change = true;
            continue;
        }
        if (change) {
            if (ch == '\\' || ch == '"' || ch == '\'' || ch == ';' || ch == '#' || ch == '`') {
                buffers.emplace_back(ch);
            } else if (ch == 'n') {
                buffers.emplace_back('\n');
            } else if (ch == 't') {
                buffers.emplace_back('\t');
            } else if (ch == 'b') {
                if (!buffers.empty()) {
                    buffers.pop_back();
                }
            } else if (ch == 'r') {
                buffers.clear();
            } else if (ch == '\n') {
                MatchSpaceAndComment(src, &p);
                continue;
            }
            change = false;
        } else {
            buffers.emplace_back(ch);
        }
    }

    if (p == src.size()) {
        result->assign(buffers.begin(), buffers.end());
        *pos = p;
        *terminatroSize = 0;
        return true;
    }
    return false;
}

bool INI::MatchPair(const std::string& src, const std::string& left, const std::string& right, std::size_t* pos, std::string* result) {
    auto p = *pos;
    if (!MatchWord(src, left, &p)) {
        return false;
    }
    std::size_t terminatroSize;
    if (!MatchParagraphToTerminator(src, { right }, &p, &terminatroSize, result)) {
        return false;
    }
    *pos = p + terminatroSize;
    return true;
}

bool INI::MatchString(const std::string& src, std::size_t* pos, std::string* result) {
    return MatchPair(src, "\"", "\"", pos, result)
        || MatchPair(src, "'", "'", pos, result)
        || MatchPair(src, "`", "`", pos, result);
}

bool INI::MatchWord(const std::string& src, const std::string& word, std::size_t* pos) {
    if (word.empty()) {
        return false;
    }
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
