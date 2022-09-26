
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

using namespace std::string_literals;

struct IniFile;

enum class TriBool { False, True, Maybe };

extern TriBool And(TriBool, TriBool);
extern TriBool Or(TriBool, TriBool);
extern TriBool Not(TriBool);
extern TriBool FromBool(bool);

extern std::string to_string(TriBool);

typedef std::size_t UL;

namespace runstate
{
	extern unsigned long long ml, mf, cf, sf, sl;
	extern bool colorize, statistic, sparse, warnings;
	extern bool debugConsidered;
	extern bool debugSearched;
	extern bool debugGeneral;
	extern bool wantClear;
	extern bool impliedAplus;
	extern bool colorLikeLinux;
	extern bool quoteFileNames;
	extern int tab, trunc;
	extern std::vector<std::string> debugConsideredList;
	extern std::vector<std::string> debugSearchedList;
}

extern std::string unParan(std::string str);
extern int getParam(std::string str, int def);
extern std::vector<std::string> readFile(std::istream&);

extern void registerAll();

void recursiveLoad(IniFile&, const std::string&);


