
#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <iosfwd>
#include <map>
#include <vector>

struct IniFile
{

	typedef std::map<std::string, std::string> Section;
	typedef std::map<std::string, Section> Headers;

	Headers all;

	void LoadFile(const std::string&);
	void LoadFile(std::istream&);

	void SaveFile(std::string_view) const;
	void SaveFile(const std::string&) const;
	void SaveFile(std::ostream&) const;

	std::string Lookup(const std::string& header, const std::string& key) const;
	std::string LookupWithDefault(const std::string& header, const std::string& key, const std::string& defaultvalue) const noexcept;
	bool HasKey(const std::string& header, const std::string& key) const noexcept;

	void AssignIfSet(const std::string& header, const std::string& key, bool& setting) const;
	void AssignIfSet(const std::string& header, const std::string& key, int& setting) const;
	void AssignIfSet(const std::string& header, const std::string& key, std::string& setting) const;

	std::vector<std::string> ListHeaders() const;
	bool HasHeader(const std::string&) const;
	std::vector<std::string> ListKeys(const std::string&) const;
};


