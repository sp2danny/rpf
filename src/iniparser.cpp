
#include "iniparser.h"

#include <iostream>
#include <fstream>

#include <boost/algorithm/string.hpp>

void IniFile::LoadFile(std::string_view fn)
{
	std::ifstream ifs(std::string{fn});
	LoadFile(ifs);
}

void IniFile::LoadFile(std::istream& is)
{
	std::string header;
	std::string line;
	while(std::getline(is, line))
	{
		boost::trim(line);
		if (line.empty()) continue;
		if (line.front() == '#') continue;
		if (line.front() == '[')
		{
			if (line.back() == ']')
			{
				header = line.substr(1, line.size()-2);
				boost::trim(header);
				//std::cerr << "new header : " << header << std::endl;
			}
			else
				throw "ini parse error";
			continue;
		}
		auto p = line.find('=');
		if (p==std::string::npos) throw "ini parse error";
		std::string key = line.substr(0, p);
		boost::trim(key);
		std::string val = line.substr(p+1);
		boost::trim(val);
		all[header][key]=val;
	}
}

void IniFile::SaveFile(std::string_view fn) const
{
	std::ofstream ofs(std::string{fn});
	SaveFile(ofs);
}

void IniFile::SaveFile(std::ostream& os) const
{
	auto writesec = [&os](const Section& sec)
	{
		for (auto&& itm : sec)
		{
			os << itm.first << "=" << itm.second << std::endl;
		}
	};

	auto iter = all.find("");
	if (iter != all.end())
	{
		writesec(iter->second);
	}
	for (auto&& sec : all)
	{
		if (sec.first.empty())
			continue;
		os << "[" << sec.first << "]" << std::endl;
		writesec(sec.second);
	}
}

std::string IniFile::Lookup(const std::string& header, const std::string& key) const
{
	auto i1 = all.find(header);
	if (i1 == all.end()) throw "no such header";
	const Section& sec = i1->second;
	auto i2 = sec.find(key);
	if (i2 == sec.end()) throw "no such key";
	return i2->second;
}

std::string IniFile::LookupWithDefault(const std::string& header, const std::string& key, const std::string& defaultvalue) const noexcept
{
	auto i1 = all.find(header);
	if (i1 == all.end()) return defaultvalue;
	const Section& sec = i1->second;
	auto i2 = sec.find(key);
	if (i2 == sec.end()) return defaultvalue;
	return i2->second;
}

bool IniFile::HasKey(const std::string& header, const std::string& key) const noexcept
{
	auto i1 = all.find(header);
	if (i1 == all.end()) return false;
	const Section& sec = i1->second;
	auto i2 = sec.find(key);
	if (i2 == sec.end()) return false;
	return true;
}

void IniFile::AssignIfSet(const std::string& header, const std::string& key, bool& setting) const
{
	auto i1 = all.find(header);
	if (i1 == all.end()) return;
	const Section& sec = i1->second;
	auto i2 = sec.find(key);
	if (i2 == sec.end()) return;
	auto str = boost::to_lower_copy(i2->second);
	/**/ if ( (str == "1") || (str == "on") || (str == "true") )
		setting = true;
	else if ( (str == "0") || (str == "off") || (str == "false") )
		setting = false;
}

void IniFile::AssignIfSet(const std::string& header, const std::string& key, int& setting) const
{
	auto i1 = all.find(header);
	if (i1 == all.end()) return;
	const Section& sec = i1->second;
	auto i2 = sec.find(key);
	if (i2 == sec.end()) return;
	auto& str = i2->second;
	setting = std::stoi(str);
}

void IniFile::AssignIfSet(const std::string& header, const std::string& key, std::string& setting) const
{
	auto i1 = all.find(header);
	if (i1 == all.end()) return;
	const Section& sec = i1->second;
	auto i2 = sec.find(key);
	if (i2 == sec.end()) return;
	setting = i2->second;
}

std::vector<std::string> IniFile::ListHeaders() const
{
	std::vector<std::string> out;
	for (auto&& seg : all)
	{
		out.push_back(seg.first);
	}
	return out;
}

bool IniFile::HasHeader(const std::string& hdr) const
{
	auto iter = all.find(hdr);
	if (iter == all.end()) return false;
	return !iter->second.empty();
}

std::vector<std::string> IniFile::ListKeys(const std::string& hdr) const
{
	auto iter = all.find(hdr);
	if (iter == all.end())
		return {};
	std::vector<std::string> out;
	for (auto&& kvp : iter->second)
	{
		out.push_back(kvp.first);
	}
	return out;
}
