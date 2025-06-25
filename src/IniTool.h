
#pragma once

#include "IniObject.h"
#include <initializer_list>

class INI {
public:
	static IniObject Parse(const std::string& src);
	static std::string Stringify(const IniObject& iniObject);

private:
	static bool MatchSection(const std::string& src, std::size_t* pos, std::string* section);
	static bool MatchKey(const std::string& src, std::size_t* pos, std::string* name);
	static bool MatchValue(const std::string& src, std::size_t* pos, std::string* value);
	static bool MatchAssign(const std::string& src, std::size_t* pos);

	static void MatchSpaceAndComment(const std::string& src, std::size_t* pos);
	static bool MatchParagraphToTerminator(const std::string& src, const std::initializer_list<std::string>& terminator, std::size_t* pos, std::size_t* terminatroSize, std::string* result);
	static bool MatchPair(const std::string& src, const std::string& left, const std::string& right, std::size_t* pos, std::string* result);
	static bool MatchString(const std::string& src, std::size_t* pos, std::string* result);
	static bool MatchWord(const std::string& src, const std::string& word, std::size_t* pos);
	static bool IsNormalizeKey(const std::string& key);
	static bool IsNormalizeKeyChar(char ch);
	static bool IsNotString(const std::string& value);
	static bool NeedEscape(char ch);

private:
	INI() = default;
};