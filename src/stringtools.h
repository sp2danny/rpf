
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <climits>
#include <cstring>
#include <cctype>

#include <boost/algorithm/string.hpp>

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

extern bool str_pat_mat(const char* str, const char* pat);
extern bool str_pat_mat(const std::string& str, const std::string& pat);

typedef std::pair<std::size_t, std::size_t> ZZP;
extern std::vector<ZZP> str_pat_mat_special(const std::string& str, const std::string& pat);

typedef std::pair<std::string, std::size_t> StrPos;

extern std::vector<StrPos> tokenify(const std::string&);

struct boyer_moore
{
	boyer_moore(std::string);
	bool match(const std::string&);
	std::vector<std::size_t> match_all(const std::string&);
	std::pair<bool, std::size_t> match_next(const std::string&, std::size_t =0);
	std::string gettext() const { return searching_for; }
private:
	std::size_t offsets[256];
	std::string searching_for;
};

template<typename Ch>
using ChEqP = bool (*)(Ch, Ch);

extern bool isEqualNoCase(char c1, char c2);

template<typename Ch = char, typename Str = std::basic_string<Ch>, ChEqP<Ch> eq = std::equal_to<Ch>{}.operator() >
struct boyer_moore_advanced
{
	boyer_moore_advanced(Str);
	bool match(const Str&);
	std::vector<std::size_t> match_all(const Str&);
	std::pair<bool, std::size_t> match_next(const Str&, std::size_t =0);
	constexpr static std::size_t SZ = 1ul << (CHAR_BIT*sizeof(Ch));
	std::string gettext() const { return searching_for; }
private:
	std::size_t offsets[SZ];
	Str searching_for;
};

template<typename Ch, typename Str, ChEqP<Ch> eq>
boyer_moore_advanced<Ch,Str,eq>::boyer_moore_advanced(Str str)
	: searching_for(std::move(str))
{
	std::size_t i, n = searching_for.size();
	for (i=0; i<SZ; ++i)
	{
		bool found = false;
		std::size_t j;
		for (j=n; j>0;)
		{
			--j;
			if (eq(searching_for[j], (Ch)i))
			{
				found = true;
				offsets[i] = (n-j)-1;
				break;
			}
		}
		if (!found)
			offsets[i] = n;
	}
}

template<typename Ch, typename Str, ChEqP<Ch> eq>
bool boyer_moore_advanced<Ch,Str,eq>::match(const Str& searching_in)
{
	return match_next(searching_in, 0).first;
}

template<typename Ch, typename Str, ChEqP<Ch> eq>
std::pair<bool, std::size_t> boyer_moore_advanced<Ch,Str,eq>::match_next(const Str& searching_in, std::size_t pos)
{
	std::size_t ls = searching_in.size();
	std::size_t n = searching_for.size();
	std::size_t i = pos + n - 1;
	while (i<ls)
	{
		std::size_t ic = (std::size_t)searching_in[i];
		auto ofs = offsets[ic];
		if (ofs)
		{
			i += ofs;
			continue;
		}
		bool found = true;
		for (std::size_t j=0; (j<n) && found; ++j)
		{
			if (!eq(searching_for[j], searching_in[i+1-n+j]))
				found = false;
		}
		if (found) return {true,i+1-n};
		++i;
	}
	return {false, 0};
}

template<typename Ch, typename Str, ChEqP<Ch> eq>
std::vector<std::size_t> boyer_moore_advanced<Ch,Str,eq>::match_all(const Str& searching_in)
{
	std::vector<std::size_t> res;
	std::size_t pos = 0;
	while (true)
	{
		auto m = match_next(searching_in, pos);
		if (!m.first) break;
		res.push_back(m.second);
		pos = m.second+1;
	}
	return res;
}

typedef boyer_moore_advanced<char, std::string, isEqualNoCase> boyer_moore_ci;

class OutputFormatter
{
public:
	OutputFormatter(std::size_t colmarg = 2, std::size_t tabs = 8, std::size_t trunc = 999);
	void LineOut(std::string);
	void ColumnsOut(std::vector<std::string>);
	template<typename... Args>
	void ColumnsOut(Args&&... args);
	void SetColmarg(std::size_t);
	void SetTabstop(std::size_t);
	void SetTruncate(std::size_t);
	void OutAndClear(bool colorize, std::ostream&);
private:
	std::size_t cm, ts, tr;
	std::vector<std::vector<std::string>> lines;
};

inline void app(std::vector<std::string>&) {}
template<typename T, typename... Args>
void app(std::vector<std::string>& ret, T&& t, Args&&... args)
{
	ret.push_back( std::forward<T>(t) );
	app(ret, std::forward<Args>(args)...);
}

template<typename... Args>
void OutputFormatter::ColumnsOut(Args&&... args)
{
	std::vector<std::string> ret;
	app(ret, std::forward<Args>(args)...);
	ColumnsOut(ret);
}


