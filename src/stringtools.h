
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

extern bool strPatMat(const char* str, const char* pat);
extern bool strPatMat(const std::string& str, const std::string& pat);

typedef std::pair<std::size_t, std::size_t> ZZP;
extern std::vector<ZZP> strPatMatSpecial(const std::string& str, const std::string& pat);

typedef std::pair<std::string, std::size_t> StrPos;

extern std::vector<StrPos> tokenify(const std::string&);

struct BoyerBoore
{
	BoyerBoore(std::string);
	bool match(const std::string&);
	std::vector<std::size_t> matchAll(const std::string&);
	std::pair<bool, std::size_t> matchNext(const std::string&, std::size_t =0);
	std::string getText() const { return mSearchingFor; }
private:
	std::size_t mOffsets[256];
	std::string mSearchingFor;
};

template<typename Ch>
using ChEqP = bool (*)(Ch, Ch);

extern bool isEqualNoCase(char c1, char c2);

template<typename Ch = char, typename Str = std::basic_string<Ch>, ChEqP<Ch> eq = std::equal_to<Ch>{}.operator() >
struct boyerMooreAdvanced
{
	boyerMooreAdvanced(Str);
	bool match(const Str&);
	std::vector<std::size_t> matchAll(const Str&);
	std::pair<bool, std::size_t> matchNext(const Str&, std::size_t =0);
	constexpr static std::size_t SZ = 1ul << (CHAR_BIT*sizeof(Ch));
	std::string getText() const { return mSearchingFor; }
private:
	std::size_t mOffsets[SZ];
	Str mSearchingFor;
};

template<typename Ch, typename Str, ChEqP<Ch> eq>
boyerMooreAdvanced<Ch,Str,eq>::boyerMooreAdvanced(Str str)
	: mSearchingFor(std::move(str))
{
	std::size_t i, n = mSearchingFor.size();
	for (i=0; i<SZ; ++i)
	{
		bool found = false;
		std::size_t j;
		for (j=n; j>0;)
		{
			--j;
			if (eq(mSearchingFor[j], (Ch)i))
			{
				found = true;
				mOffsets[i] = (n-j)-1;
				break;
			}
		}
		if (!found)
			mOffsets[i] = n;
	}
}

template<typename Ch, typename Str, ChEqP<Ch> eq>
bool boyerMooreAdvanced<Ch,Str,eq>::match(const Str& searchingIn)
{
	return matchNext(searchingIn, 0).first;
}

template<typename Ch, typename Str, ChEqP<Ch> eq>
std::pair<bool, std::size_t> boyerMooreAdvanced<Ch,Str,eq>::matchNext(const Str& searchingIn, std::size_t pos)
{
	std::size_t ls = searchingIn.size();
	std::size_t n  = mSearchingFor.size();
	std::size_t i  = pos + n - 1;
	while (i < ls)
	{
		std::size_t ic = (std::size_t)searchingIn[i];
		auto ofs = mOffsets[ic];
		if (ofs)
		{
			i += ofs;
			continue;
		}
		bool found = true;
		for (std::size_t j=0; (j<n) && found; ++j)
		{
			if (!eq(mSearchingFor[j], searchingIn[i+1-n+j]))
				found = false;
		}
		if (found) return {true, i+1-n};
		++i;
	}
	return {false, 0};
}

template<typename Ch, typename Str, ChEqP<Ch> eq>
std::vector<std::size_t> boyerMooreAdvanced<Ch,Str,eq>::matchAll(const Str& searchingIn)
{
	std::vector<std::size_t> res;
	std::size_t pos = 0;
	while (true)
	{
		auto m = matchNext(searchingIn, pos);
		if (!m.first) break;
		res.push_back(m.second);
		pos = m.second + 1;
	}
	return res;
}

using boyerMooreCI = boyerMooreAdvanced<char, std::string, isEqualNoCase>;

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


