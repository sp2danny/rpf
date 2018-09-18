
#include "common.h"
#include "execute.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

bool Execute_BM(Results& res, const Operator& op)
{
	assert(op.type == opBM);
	if (!op.bm)
	{
		op.bm = boyer_moore(op.pattern);
	}
	Match m;
	UL ln = 0;
	for (auto&& l : res.file.lines())
	{
		auto lst = op.bm->match_all(l);
		for (auto&& p : lst)
			m.add_full_match(ln, p, p+op.pattern.size());
		++ln;
	}
	res.matches.push_back(std::move(m));
	return true;
}

bool Execute_BMCI(Results& res, const Operator& op)
{
	assert(op.type == opBMCI);
	if (!op.cibm)
	{
		op.cibm = boyer_moore_ci(op.pattern);
	}
	Match m;
	UL ln = 0;
	for (auto&& l : res.file.lines())
	{
		auto lst = op.cibm->match_all(l);
		for (auto&& p : lst)
			m.add_full_match(ln, p, p+op.pattern.size());
		++ln;
	}
	res.matches.push_back(std::move(m));
	return true;
}

#define NEO "not enough operands"

bool Execute_Invert(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opInvert);
	if (res.matches.size() < 1)
	{
		throw "operator invert: " NEO;
	}
	Match& m = res.matches.back();
	if (!m.match()) return true;
	UL i, n = res.file.lines().size();
	Lines newlines;
	for (i=0; i<n; ++i)
	{
		if (!m.have_line(i))
			newlines[i];
	}
	m.modifiable_lines().swap(newlines);
	return true;
}

bool Execute_Swap(Results& res, const Operator& op)
{
	assert(op.type == opSwap);
	if (res.matches.size() < (size_t)op.parameter)
	{
		throw "operator swap: " NEO;
	}
	auto itr1 = res.matches.end() - 1;
	auto itr2 = res.matches.end() - op.parameter;
	std::swap(*itr1, *itr2);
	return true;
}

bool Execute_Ident(Results& res, const Operator& op)
{
	assert(op.type == opIdent);
	Match m;
	UL ln = 0;
	for (auto&& l : res.file.lines())
	{
		auto tokens = tokenify(l);
		for (auto&& t : tokens)
			if (t.first == op.pattern)
				m.add_full_match(ln, t.second, t.second+op.pattern.size());
		++ln;
	}
	res.matches.push_back(std::move(m));
	return true;
}

bool Execute_IdentCI(Results& res, const Operator& op)
{
	assert(op.type == opIdentCI);
	Match m;
	UL ln = 0;
	auto pat = to_lower_copy(op.pattern);
	for (auto&& l : res.file.lines())
	{
		auto tokens = tokenify(l);
		for (auto&& t : tokens)
			if (to_lower_copy(t.first) == pat)
				m.add_full_match(ln, t.second, t.second+op.pattern.size());
		++ln;
	}
	res.matches.push_back(std::move(m));
	return true;
}

bool Execute_Sub(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opSub);
	if (res.matches.size() < 2)
	{
		throw "operator sub: " NEO;
	}
	Match m2 = std::move(res.matches.back()); res.matches.pop_back();
	Match m1 = std::move(res.matches.back()); res.matches.pop_back();

	if (m1.lines().empty() || !m1.match())
	{
		res.matches.emplace_back();
		return true;
	}
	Match m{true, {}};
	m.modifiable_lines() = move(m1.lines());
	for (auto&& l : m2.lines())
	{
		m.modifiable_lines().erase(l.first);
	}
	m.match(!m.lines().empty());
	res.matches.push_back(std::move(m));
	return true;
}

bool Execute_True(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opTrue);
	res.matches.push_back({true, {}});
	return true;
}

bool Execute_Dir(Results& res, const Operator& op)
{
	assert(op.type == opDir);
	if (str_pat_mat(to_lower_copy(res.file.path), to_lower_copy(op.pattern)))
		res.matches.push_back({true,{}});
	else
		res.matches.emplace_back();
	return true;
}

bool Execute_CppOnly(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opCppOnly);
	if (str_pat_mat(to_lower_copy(res.file.name), to_lower_copy(op.pattern)))
	{
		res.matches.push_back({true, {}});
		res.file.cpponly = true;
	} else {
		res.matches.push_back({false, {}});
	}
	return true;
}

bool Execute_File(Results& res, const Operator& op)
{
	assert(op.type == opFile);
	if (str_pat_mat(to_lower_copy(res.file.name), to_lower_copy(op.pattern)))
		res.matches.push_back({true, {}});
	else
		res.matches.push_back({false, {}});
	return true;
}

bool Execute_Line(Results& res, const Operator& op)
{
	assert(op.type == opLine);
	Match m;
	UL ln = 0;
	for (auto&& l : res.file.lines())
	{
		if (str_pat_mat(l, op.pattern))
		{
			auto vzzp = str_pat_mat_special(l, op.pattern);
			if (vzzp.empty())
				m.add_simple_match(ln);
			else
				for(auto&& zzp : vzzp)
					m.add_full_match(ln, zzp.first, zzp.second);
		}
		++ln;
	}
	res.matches.push_back(std::move(m));
	return true;
}

bool Execute_LineCI(Results& res, const Operator& op)
{
	assert(op.type == opLineCI);
	Match m;
	UL ln = 0;
	std::string pat = to_lower_copy(op.pattern);
	for (auto&& l : res.file.lines())
	{
		auto lcl = to_lower_copy(l);
		if (str_pat_mat(lcl, pat))
		{
			auto vzzp = str_pat_mat_special(lcl, pat);
			if (vzzp.empty())
				m.add_simple_match(ln);
			else
				for(auto&& zzp : vzzp)
					m.add_full_match(ln, zzp.first, zzp.second);
		}
		++ln;
	}
	res.matches.push_back(std::move(m));
	return true;
}

bool Execute_Regexp(Results& res, const Operator& op)
{
	assert(op.type == opRegexp);
	std::regex e{op.pattern};
	Match m;
	UL ln = 0;
	typedef std::string::const_iterator SCI;
	for (auto&& l : res.file.lines())
	{
		SCI lb = l.cbegin();
		SCI le = l.cend();
		std::smatch sm;
		while (regex_search(lb, le, sm, e))
		{
			if (sm[0].matched)
			{
				SCI beg = sm[0].first;
				SCI end = sm[0].second;
				m.add_full_match(ln, beg-lb, end-lb);
				lb = std::next(beg);
			}
			else
				break;
		}
		++ln;
	}
	res.matches.push_back(std::move(m));
	return true;
}

bool Execute_Bail(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opBail);
	if (res.matches.size() < 1)
	{
		throw "operator bail: " NEO;
	}
	if (!res.matches.back().match())
	{
		res.matches.clear();
		res.matches.emplace_back();
		return false;
	}
	return true;
}

bool Execute_Pop(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opPop);
	if (res.matches.size() < 1)
	{
		throw "operator pop: " NEO;
	}
	res.matches.pop_back();
	return true;
}

bool Execute_Negate(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opNegate);
	if (res.matches.size() < 1)
	{
		throw "operator negate: " NEO;
	}
	Match m = std::move(res.matches.back()); res.matches.pop_back();
	res.matches.push_back({!m.match(), {}});
	return true;
}

bool Execute_Dup(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opDup);
	if (res.matches.size() < 1)
	{
		throw "operator dup: " NEO;
	}
	Match m = res.matches.back();
	res.matches.push_back(m);
	return true;
}

bool Execute_And(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opAnd);
	if (res.matches.size() < 2)
	{
		throw "operator and: " NEO;
	}
	Match m2 = std::move(res.matches.back()); res.matches.pop_back();
	Match m1 = std::move(res.matches.back()); res.matches.pop_back();
	if (m1.match() && m2.match())
	{
		Match m;
		m.match(true);
		m.modifiable_lines() = std::move(m1.modifiable_lines());
		for (auto&& l : m2.lines())
		{
			if (l.second.empty())
				m.add_simple_match(l.first);
			else
				for (auto&& x : l.second)
					m.add_full_match(l.first, x);
		}
		res.matches.push_back(std::move(m));
	} else {
		res.matches.emplace_back();
	}
	return true;
}

bool Execute_Or(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opOr);
	if (res.matches.size() < 2)
	{
		throw "operator or: " NEO;
	}
	Match m2 = std::move(res.matches.back()); res.matches.pop_back();
	Match m1 = std::move(res.matches.back()); res.matches.pop_back();
	if (m1.match() || m2.match())
	{
		Match m;
		m.match(true);
		m.modifiable_lines() = std::move(m1.modifiable_lines());
		for (auto&& l : m2.lines())
		{
			if (l.second.empty())
				m.add_simple_match(l.first);
			else
				for (auto&& x : l.second)
					m.add_full_match(l.first, x);
		}
		res.matches.push_back(std::move(m));
	} else {
		res.matches.emplace_back();
	}
	return true;
}

bool Execute_Near(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opNear);
	if (res.matches.size() < 2)
	{
		throw "operator near: " NEO;
	}
	Match m2 = std::move(res.matches.back()); res.matches.pop_back();
	Match m1 = std::move(res.matches.back()); res.matches.pop_back();

	if ((!m1.match()) || (!m2.match()))
	{
		res.matches.emplace_back();
		return true;
	}

	Match m;
	for (auto&& l1 : m1.lines())
	{
		for(auto&& l2 : m2.lines())
		{
			auto diff = (l2.first>l1.first) ? l2.first-l1.first : l1.first-l2.first;
			if (int(diff) <= op.parameter)
			{
				m.add_simple_match(l1.first);
				m.add_simple_match(l2.first);
				for (auto&& x : l1.second)
					m.add_full_match(l1.first, x);
				for (auto&& x : l2.second)
					m.add_full_match(l2.first, x);
			}
		}
	}
	res.matches.push_back(std::move(m));
	return true;
}

bool Execute_Clear(Results& res, [[maybe_unused]] const Operator& op)
{
	assert(op.type == opClear);
	res.matches.clear();
	return true;
}

bool Execute_Mod(Results& res, const Operator& op)
{
	assert(op.type == opMod);
	if (!op.tim)
	{
		op.tim = parse_date_from_string(op.pattern);
	}

	auto filename = res.file.path + "/" + res.file.name;
	auto filetime = get_modification_time_from_file(filename);

	Match m;
	m.match(filetime >= *op.tim);
	res.matches.push_back(std::move(m));

	return true;
}

