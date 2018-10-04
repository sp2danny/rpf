
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

enum TriBool { tb_false, tb_true, tb_maybe };

extern TriBool And(TriBool, TriBool);
extern TriBool Or(TriBool, TriBool);
extern TriBool Not(TriBool);
extern TriBool FromBool(bool);

typedef std::size_t UL;

namespace runstate
{
	extern unsigned long long ml, mf, cf, sf, sl;
	extern bool colorize, statistic, sparse, warnings;
	extern bool debug_considered;
	extern bool debug_searched;
	extern bool debug_general;
	extern int tab, trunc;
	extern std::vector<std::string> debug_considered_list;
	extern std::vector<std::string> debug_searched_list;
}

extern std::string unparan(std::string str);
extern int getparam(std::string str, int def);
extern std::vector<std::string> readfile(std::istream&);

extern void register_all();


