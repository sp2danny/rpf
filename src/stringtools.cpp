
#include "stringtools.h"

#include <locale>
#include <cctype>
#include <iostream>
#include <utility>

using namespace std::string_literals;

bool isEqualNoCase(char c1, char c2)
{
	return std::tolower(c1) == std::tolower(c2);
}

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
	return {false, 0};
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

// ----------------------------------------------------------------------------

#include "platform.h"

OutputFormatter::OutputFormatter(std::size_t colmarg, std::size_t tabs, std::size_t trunc)
{
	SetColmarg(colmarg);
	SetTabstop(tabs);
	SetTruncate(trunc);
}

void OutputFormatter::LineOut(std::string s)
{
	lines.emplace_back();
	lines.back().push_back( std::move(s) );
}

void OutputFormatter::ColumnsOut(std::vector<std::string> vs)
{
	lines.push_back( std::move(vs) );
}

void OutputFormatter::SetColmarg  (std::size_t colmarg) { cm = colmarg; }
void OutputFormatter::SetTabstop  (std::size_t tabs   ) { ts = tabs;    }
void OutputFormatter::SetTruncate (std::size_t trunc  ) { tr = trunc;   }

const std::vector<std::string> fmt = { "%b%", "%g%", "%r%", "%n%" };

std::pair<bool, std::size_t> is_formatter(std::string str)
{
	if (str.length() != 3) return {false, 0};
	std::size_t i = 0;
	for (auto&& f : fmt)
	{
		if (str == f) return {true, i};
		++i;
	}
	return {false, 0};
}

void colorize_out(bool colorize, const std::string& str, std::ostream& out, UL trunc)
{
	std::size_t l=0, i=0, sz = str.length();
	while (i < sz)
	{
		if (l == trunc) { out << '\\'; ++l; }
		char c = str[i];
		if (c=='%')
		{
			auto cut = str.substr(i,3);
			auto [ok, idx] = is_formatter(cut);
			if (!ok)
			{
				if (l < trunc)
					out << c;
				++i; ++l;
			} else {
				if (colorize) switch (idx)
				{
					case 0: platform::MakeHighlight(); break;
					case 1: platform::MakeGreen();     break;
					case 2: platform::MakeRed();       break;
					case 3: platform::MakeNormal();    break;
				}
				i += 3;
			}
		}
		else
		{
			if (l < trunc)
				out << c;
			++i; ++l;
		}
	}
	out << std::endl;
}

std::size_t decolorize_length(std::string str)
{
	for (auto&& f : fmt)
	{
		while (true)
		{
			auto p = str.find(f);
			if (p == std::string::npos) break;
			str.replace(p, 3, "");
		}
	}
	return str.length();
}

std::string tabify_string(const std::string& str, std::size_t tabs)
{
	std::string ret;
	std::size_t i=0, l=0, n = str.length();
	while (i<n)
	{
		char c = str[i];
		if (c=='%')
		{
			auto cut = str.substr(i,3);
			auto [ok, idx] = is_formatter(cut);
			if (ok)
			{
				i += 3;
				ret += fmt[idx];
			}
			else
			{
				ret += c;
				i += 1; l += 1;
			}
		}
		else if (c=='\t')
		{
			ret += ' '; l += 1;
			while ((l%tabs) != 0)
			{
				ret += ' '; l += 1;
			}
			i += 1;
		}
		else
		{
			ret += c;
			i += 1; l += 1;
		}
	}
	return ret;
}

void OutputFormatter::OutAndClear(bool colorize, std::ostream& out)
{
	std::vector<std::size_t> col_stops;

	for (auto&& line : lines)
	{
		std::size_t i, n=line.size();
		for (i=0; i<(n-1); ++i)
		{
			auto len = decolorize_length(line[i]) + cm;
			while (i >= col_stops.size())
				col_stops.push_back(cm);
			if (col_stops[i] < len)
				col_stops[i] = len;
		}
	}

	for (auto&& line : lines)
	{
		auto sz = line.size();
		if (sz == 0) { out << std::endl; continue; }
		if (sz == 1) { colorize_out(colorize, tabify_string(line.back(), ts), out, tr); continue; }
		std::string outstr;
		for (auto j=0ul; j<cm; ++j) outstr += " ";
		for (auto i=0ul; i<(sz-1); ++i)
		{
			auto&& str = line[i];
			auto lsz = decolorize_length(str);
			outstr += tabify_string(str, 1);
			if (col_stops.size() > i)
				for (auto j=lsz; j<col_stops[i]; ++j)
					outstr += " ";
		}
		outstr += tabify_string(line.back(), ts);
		colorize_out(colorize, outstr, out, tr);
	}

	lines.clear();
}





