
#include "stringtools.h"

#include <locale>
#include <cctype>
#include <iostream>
#include <utility>

using namespace std::string_literals;

bool str_pat_mat(const char* str, const char* pat)
{
	bool se = (str[0] == 0);
	bool pe = (pat[0] == 0);
	if (se && pe)
		return true;
	if (pe)
		return false;
	if ((pat[0]=='*') && (pat[1]==0))
		return true;
	if (se)
		return false;
	switch (pat[0])
	{
	case '?':
		return str_pat_mat(str+1, pat+1);
	case '*':
		return str_pat_mat(str, pat+1) || str_pat_mat(str+1, pat);
	default:
		return (str[0]==pat[0]) && str_pat_mat(str+1, pat+1);
	}
}

bool str_pat_mat(const std::string& str, const std::string& pat)
{
	return
		str_pat_mat(str.c_str(), pat.c_str());
}

//typedef std::pair<std::size_t, std::size_t> ZZP;

/*

struct PMS
{
	bool base_value = false;
	std::vector<char> inout;
	explicit operator bool() const { return base_value; }
	PMS& operator+=(bool b) { inout.push_back(b); return *this; }
	PMS& operator+=(const PMS& pms) { inout.insert(inout.end(), pms.inout.begin(), pms.inout.end()); return *this; }
	PMS& set(bool b) { base_value = b; return *this; }
	PMS operator+(bool b) { PMS res = *this; res+=b; return res; }
};

PMS str_pat_mat_special_impl(const char* str, const char* pat, PMS in)
{

	bool se = (str[0] == 0);
	bool pe = (pat[0] == 0);
	if (se && pe)
		return in.set(true);
	if (pe)
		return in.set(false);
	if ((pat[0]=='*') && (pat[1]==0))
		return in.set(true);
	if (se)
		return in.set(false);
	switch (pat[0])
	{
	case '?':
		in += false;
		return str_pat_mat_special_impl(str+1, pat+1, in+false);
	case '*':
		{
			PMS pms = str_pat_mat_special_impl(str, pat+1, in);
			if (pms) return pms;
			return str_pat_mat_special_impl(str+1, pat, in+false) ;
		}
	default:
		{
			if (str[0]!=pat[0])
				return in.set(false);
			return str_pat_mat_special_impl(str+1, pat+1, in+true);
		}
	}

	return in.set(false);
}

std::vector<ZZP> str_pat_mat_special(const std::string& str, const std::string& pat)
{
	std::size_t i, n = str.size();
	
	std::vector<ZZP> res;
	PMS pms;
	pms = str_pat_mat_special_impl(str.c_str(), pat.c_str(), pms);
	if (pms)
	{
		while (pms.inout.size()<n) pms+=false;
		bool wasin = false;
		std::size_t inp;
		for (i=0; i<n; ++i)
		{
			bool isin = pms.inout[i];
			if (isin && !wasin)
			{
				wasin = true;
				inp = i;
			}
			else if (wasin && !isin)
			{
				res.push_back({inp, i});
				wasin = false;
			}
		}
		if (wasin)
			res.push_back({inp,i});
	}
	return res;
}
*/

bool str_pat_mat_special_impl(const char* str, const char* pat, char* mat)
{
	bool se = (str[0] == 0);
	bool pe = (pat[0] == 0);
	if (se && pe)
		return true;
	if (pe)
		return false;
	if ((pat[0]=='*') && (pat[1]==0))
	{
		while (*str++)
			*mat++ = 0;
		return true;
	}
	if (se)
		return false;
	switch (pat[0])
	{
	case '?':
		*mat = 0;
		return str_pat_mat_special_impl(str+1, pat+1, mat+1);
	case '*':
		{
			bool ok = str_pat_mat_special_impl(str, pat+1, mat);
			if (ok || (pat[1]=='*')) return ok;
			*mat = 0;
			return str_pat_mat_special_impl(str+1, pat, mat+1) ;
		}
	default:
		{
			if (str[0]!=pat[0])
				return false;
			*mat = 1;
			return str_pat_mat_special_impl(str+1, pat+1, mat+1);
		}
	}

	return false;
}

std::vector<ZZP> str_pat_mat_special(const std::string& str, const std::string& pat)
{
	std::vector<char> match;
	std::size_t i, n = str.size();
	match.assign(n, 0);
	std::vector<ZZP> res;
	bool ok = str_pat_mat_special_impl(str.c_str(), pat.c_str(), match.data());
	if (ok)
	{
		bool wasin = false;
		std::size_t inp;
		for (i=0; i<n; ++i)
		{
			bool isin = match[i];
			if (isin && !wasin)
			{
				wasin = true;
				inp = i;
			}
			else if (wasin && !isin)
			{
				res.push_back({inp, i});
				wasin = false;
			}
		}
		if (wasin)
			res.push_back({inp,i});
	}
	return res;
}

std::vector<StrPos> tokenify(const std::string& str)
{
	std::vector<StrPos> res;
	StrPos id{"",0};
	bool last_ident = false;
	std::size_t sz = str.size();
	for (std::size_t p=0; p<sz; ++p)
	{
		char c = str[p];
		bool is_ws = isspace(c);
		if (is_ws)
		{
			if (!id.first.empty()) res.push_back(id);
			id.first.clear();
			last_ident = false;
			continue;
		}
		bool is_alp = isalpha(c) || (c=='_');
		if (is_alp && !last_ident)
		{
			if (!id.first.empty()) res.push_back(id);
			id.first = ""s + c;
			id.second = p;
			last_ident = true;
			continue;
		}
		bool is_num = isdigit(c);
		if (last_ident && (is_alp || is_num))
		{
			if (id.first.empty()) id.second=p;
			id.first += c;
			continue;
		}
		if (!id.first.empty()) res.push_back(id);
		res.push_back({""s + c, p});
		last_ident = false;
		id.first.clear();
	}
	if (!id.first.empty()) res.push_back(id);
	return res;
}

boyer_moore::boyer_moore(std::string str)
	: searching_for(std::move(str))
{
	size_t i, n = searching_for.size();
	for (i=0; i<256; ++i)
		offsets[i] = n;
	for (i=n; i>0;)
	{
		unsigned char uc = (unsigned char)searching_for[--i];
		if (offsets[uc] == n)
			offsets[uc] = (n-i)-1;
	}
}

bool boyer_moore::match(const std::string& searching_in)
{
	return match_next(searching_in, 0).first;
}

std::pair<bool, std::size_t> boyer_moore::match_next(const std::string& searching_in, std::size_t pos)
{
	std::size_t ls = searching_in.size();
	std::size_t n = searching_for.size();
	std::size_t i = pos + n - 1;
	while (i<ls)
	{
		unsigned char uc = (unsigned char)searching_in[i];
		auto ofs = offsets[uc];
		if (ofs)
		{
			i += ofs;
			continue;
		}
		bool found = true;
		for (size_t j=0; (j<n) && found; ++j)
		{
			if (searching_for[j] != searching_in[i+1-n+j])
				found = false;
		}
		if (found) return {true, i+1-n};
		++i;
	}
	return {false,0};
}

std::vector<std::size_t> boyer_moore::match_all(const std::string& searching_in)
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

