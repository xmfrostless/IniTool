
#pragma once

#include "IniObject.h"
#include <memory>
#include <string>

class IniTool {
public:
	std::shared_ptr<IniObject> Parse(const std::string& src) const;
	std::string ToString(std::shared_ptr<IniObject> iniObject) const;

private:
	bool MatchSection(const std::string& src, std::size_t* pos, std::string* section) const;
	bool MatchValue(const std::string& src, std::size_t* pos, std::string* value) const;
	bool MatchNameString(const std::string& src, std::size_t* pos, std::string* name) const;
	void MatchSpace(const std::string& src, bool checkLineEnd, std::size_t* pos) const;
	bool IsBool(const std::string& src) const;
};