
#pragma once

#include "IniObject.h"
#include <memory>
#include <string>

class IniTool {
public:
	IniObject Parse(const std::string& src);
	std::string Stringify(const IniObject& iniObject);

private:
	bool MatchSection(const std::string& src, std::size_t* pos, std::string* section);
	bool MatchKey(const std::string& src, std::size_t* pos, std::string* name);
	bool MatchValue(const std::string& src, std::size_t* pos, std::string* value);
	bool MatchAssign(const std::string& src, std::size_t* pos);

	void MatchSpaceAndComment(const std::string& src, std::size_t* pos);
	bool MatchPair(const std::string& src, const std::string& left, const std::string& right, std::size_t* pos, std::string* result);
	bool MatchString(const std::string& src, std::size_t* pos, std::string* result);
	bool MatchWord(const std::string& src, const std::string& word, std::size_t* pos);
	bool IsBool(const std::string& src) const;
};