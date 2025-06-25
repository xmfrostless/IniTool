#include "IniObject.h"
#include <regex>

IniObject::IniObject(std::unordered_map<std::string, std::unordered_map<std::string, std::string>>&& data)
	: _dataMap(std::move(data)) {
}

IniObject::IniObject(const IniObject& ini)
	: _dataMap(ini._dataMap) {
}

IniObject::IniObject(IniObject&& ini)
	: _dataMap(std::move(ini._dataMap)) {
}

IniObject& IniObject::operator=(const IniObject& ini) {
	this->_dataMap = ini._dataMap;
	return *this;
}
IniObject& IniObject::operator=(IniObject&& ini) {
	this->_dataMap = std::move(ini._dataMap);
	return *this;
}

std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& IniObject::Data() {
	return _dataMap;
}
const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& IniObject::Data() const {
	return _dataMap;
}
static std::unordered_map<std::string, std::string> _emptyData;
std::unordered_map<std::string, std::string>& IniObject::SectionData(const std::string& section) {
	auto ite = _dataMap.find(section);
	if (ite != _dataMap.end()) {
		return ite->second;
	}
	return _emptyData;
}
const std::unordered_map<std::string, std::string>& IniObject::SectionData(const std::string& section) const {
	auto ite = _dataMap.find(section);
	if (ite != _dataMap.end()) {
		return ite->second;
	}
	return _emptyData;
}

bool IniObject::HasSection(const std::string& section) const {
	return _dataMap.find(section) != _dataMap.end();
}

bool IniObject::HasKey(const std::string& section, const std::string& key) const {
	auto iteSection = _dataMap.find(section);
	if (iteSection != _dataMap.end()) {
		return iteSection->second.find(key) != iteSection->second.end();
	}
	return false;
}

const std::string& IniObject::GetString(const std::string& section, const std::string& key, const std::string& defaultValue) const {
	auto iteSection = _dataMap.find(section);
	if (iteSection != _dataMap.end()) {
		auto ite = iteSection->second.find(key);
		if (ite != iteSection->second.end()) {
			return ite->second;
		}
	}
	return defaultValue;
}
double IniObject::GetDouble(const std::string& section, const std::string& key, double defaultValue) const {
	auto iteSection = _dataMap.find(section);
	if (iteSection != _dataMap.end()) {
		auto ite = iteSection->second.find(key);
		if (ite != iteSection->second.end()) {
			std::size_t pos;
			double ret = std::stod(ite->second, &pos);
			if (pos > 0) {
				return ret;
			}
		}
	}
	return defaultValue;
}
float IniObject::GetFloat(const std::string& section, const std::string& key, float defaultValue) const {
	auto iteSection = _dataMap.find(section);
	if (iteSection != _dataMap.end()) {
		auto ite = iteSection->second.find(key);
		if (ite != iteSection->second.end()) {
			try {
				std::size_t pos;
				float ret = std::stof(ite->second, &pos);
				if (pos > 0) {
					return ret;
				}
			} catch (...) {
				return defaultValue;
			}
		}
	}
	return defaultValue;
}
int IniObject::GetInt(const std::string& section, const std::string& key, int defaultValue) const {
	auto iteSection = _dataMap.find(section);
	if (iteSection != _dataMap.end()) {
		auto ite = iteSection->second.find(key);
		if (ite != iteSection->second.end()) {
			try {
				std::size_t pos;
				int ret = std::stoi(ite->second, &pos);
				if (pos > 0) {
					return ret;
				}
			} catch (...) {
				return defaultValue;
			}
		}
	}
	return defaultValue;
}
long long IniObject::GetInt64(const std::string& section, const std::string& key, long long defaultValue) const {
	auto iteSection = _dataMap.find(section);
	if (iteSection != _dataMap.end()) {
		auto ite = iteSection->second.find(key);
		if (ite != iteSection->second.end()) {
			try {
				std::size_t pos;
				long long ret = std::stoll(ite->second, &pos);
				if (pos > 0) {
					return ret;
				}
			} catch (...) {
				return defaultValue;
			}
		}
	}
	return defaultValue;
}
bool IniObject::GetBool(const std::string& section, const std::string& key, bool defaultValue) const {
	auto iteSection = _dataMap.find(section);
	if (iteSection != _dataMap.end()) {
		auto ite = iteSection->second.find(key);
		if (ite != iteSection->second.end()) {
			const auto& value = ite->second;
			if (IsBool(value)) {
				return value.size() > 4;
			}
			return defaultValue;
		}
	}
	return defaultValue;
}

void IniObject::Set(const std::string& section, const std::string& key, const std::string& value) {
	_dataMap[section][key] = value;
}
void IniObject::Set(const std::string& section, const std::string& key, double value) {
	_dataMap[section][key] = std::to_string(value);
}
void IniObject::Set(const std::string& section, const std::string& key, float value) {
	_dataMap[section][key] = std::to_string(value);
}
void IniObject::Set(const std::string& section, const std::string& key, int value) {
	_dataMap[section][key] = std::to_string(value);
}
void IniObject::Set(const std::string& section, const std::string& key, long long value) {
	_dataMap[section][key] = std::to_string(value);
}
void IniObject::Set(const std::string& section, const std::string& key, bool value) {
	_dataMap[section][key] = value ? "true" : "false";
}

bool IniObject::IsNumber(const std::string& src) {
	return std::regex_match(src, std::regex(R"(^[-+]?(?:[0-9]+)?(?:\.[0-9]+)?(?:[eE][-+]?[0-9]+)?$)"));
}

bool IniObject::IsBool(const std::string& src) {
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