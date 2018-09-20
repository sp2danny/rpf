
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstddef>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <functional>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <regex>
#include <string>
#include <stdexcept>
#include <set>
#include <utility>
#include <vector>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include "clone_ptr.hpp"

#include "stringtools.h"

using namespace std::string_literals;

enum TriBool { tb_false, tb_true, tb_maybe };

extern TriBool And(TriBool, TriBool);
extern TriBool Or(TriBool, TriBool);
extern TriBool Not(TriBool);
extern TriBool FromBool(bool);

typedef boyer_moore_advanced<char, std::string, isEqualNoCase> boyer_moore_ci;

typedef std::size_t UL;

typedef std::pair<UL, UL> MIP;
typedef std::vector<MIP> MMIP;
typedef std::map<UL, MMIP> Lines;
typedef Lines::iterator LIter;
typedef Lines::const_iterator LCIter;

struct LineMatch
{
	LineMatch() = default;
	LineMatch(bool m, const Lines& l) : m_match(m), m_lines(l) {}
	LineMatch(bool m, Lines&& l) : m_match(m), m_lines(std::move(l)) {}
	bool match() const;
	void match(bool);
	const Lines& lines() const;
	Lines& modifiable_lines();
	void add_simple_match(UL l);
	void add_full_match(UL l, UL b, UL e);
	void add_full_match(UL l, MIP mip);
	void add_full_match(LIter li, UL b, UL e);
	void add_full_match(LIter li, MIP mip);
	bool have_line(UL) const;
	bool have_char(UL,UL) const;
	bool have_char(LCIter,UL) const;
private:
	bool m_match = false;
	Lines m_lines;
};

typedef std::vector<LineMatch> LineMatchStack;

typedef std::vector<TriBool> FileMatchStack;

struct Operator;

typedef clone_ptr<Operator> (*OperatorMaker)(std::string);

struct File;

struct Operator
{
	Operator() = default;
	virtual ~Operator() = default;
	virtual char MyChar() = 0;
	virtual void Create(std::string) = 0;
	virtual Operator* clone() =0;

	static void Register(char, OperatorMaker);
	static clone_ptr<Operator> DispatchCreate(std::string);

	virtual void Register() = 0;

	virtual void MatchDir   ( File&, FileMatchStack& ) = 0;
	virtual void MatchFile  ( File&, FileMatchStack& ) = 0;
	virtual void MatchLines ( File&, LineMatchStack& ) = 0;

private:
	static std::map<char, OperatorMaker> createMap;

};

typedef std::vector<clone_ptr<Operator>> OperatorStack;

TriBool ExecuteAllDir  ( OperatorStack&, FileMatchStack& );
TriBool ExecuteAllFile ( OperatorStack&, FileMatchStack& );
bool    ExecuteAllLine ( OperatorStack&, LineMatchStack& );

struct File
{
	std::string path;
	std::string name;
	std::vector<std::string>& lines();
	bool cpponly;
private:
	std::vector<std::string> m_lines;
	std::vector<std::string> m_stripped;
	bool m_loaded = false;
};

namespace runstate
{
	extern unsigned long long ml, mf, cf, sf, sl;
	extern bool colorize, statistic, sparse;
}

extern std::string unparan(std::string str);
extern int getparam(std::string str, int def);
extern std::vector<std::string> readfile(std::istream&);

extern void register_all();

// platform stuff

extern void MakeRed();
extern void MakeGreen();
extern void MakeHighlight();
extern void MakeNormal();

struct RDE_Item
{
	std::string dir_name;
	std::string file_name;
};

struct RDE
{
	RDE() = default;
	RDE(std::string dir);
	~RDE();

	RDE& operator=(RDE&&) noexcept = default;
	RDE(RDE&&) noexcept = default;

	void swap(RDE& other) noexcept { pimpl.swap(other.pimpl); }

	std::unique_ptr<RDE_Item> getNext();
	void skipDir();

private:
	struct PIMPL;
	typedef std::unique_ptr<PIMPL> UPIMPL;
	UPIMPL pimpl;
};

extern std::time_t parse_date_from_string(std::string);
extern std::time_t get_modification_time_from_file(std::string);
extern bool        stdout_isatty();



