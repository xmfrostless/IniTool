#pragma once

#include <unordered_map>
#include <string>

class IniObject {
public:
	IniObject() = default;
	IniObject(std::unordered_map<std::string, std::unordered_map<std::string, std::string>>&& data);
	IniObject(const IniObject& ini);
	IniObject(IniObject&& ini);

	std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& Data();
	const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& Data() const;
	std::unordered_map<std::string, std::string>& SectionData(const std::string& section);
	const std::unordered_map<std::string, std::string>& SectionData(const std::string& section) const;

	bool HasSection(const std::string& section) const;
	bool HasKey(const std::string& section, const std::string& key) const;

	const std::string& GetString(const std::string& section, const std::string& key, const std::string& defaultValue = "") const;
	double GetDouble(const std::string& section, const std::string& key, double defaultValue = 0) const;
	float GetFloat(const std::string& section, const std::string& key, float defaultValue = 0) const;
	int GetInt(const std::string& section, const std::string& key, int defaultValue = 0) const;
	long long GetInt64(const std::string& section, const std::string& key, long long defaultValue = 0) const;
	bool GetBool(const std::string& section, const std::string& key, bool defaultValue = false) const;

	void Set(const std::string& section, const std::string& key, const std::string& value);
	void Set(const std::string& section, const std::string& key, double value);
	void Set(const std::string& section, const std::string& key, float value);
	void Set(const std::string& section, const std::string& key, int value);
	void Set(const std::string& section, const std::string& key, long long value);
	void Set(const std::string& section, const std::string& key, bool value);

private:
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> _dataMap;
};