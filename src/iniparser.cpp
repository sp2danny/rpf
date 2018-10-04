
#include "iniparser.h"

/*
struct IniFile
{

	typedef std::map<std::string, std::string> Section;
	typedef std::map<std::string, Section> Headers;

	Headers all;

	void LoadFile(const std::string&);
	void LoadFile(std::istream&);

	void SaveFile(const std::string&) const;
	void SaveFile(std::ostream&) const;
};
*/

#include <iostream>
#include <fstream>

#include <boost/algorithm/string.hpp>


void IniFile::LoadFile(const std::string& fn)
{
	std::ifstream ifs(fn);
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
				std::cerr << "new header : " << header << std::endl;
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

void IniFile::SaveFile(const std::string& fn) const
{
	std::ofstream ofs(fn);
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






