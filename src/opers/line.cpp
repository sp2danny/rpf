
#include "../operators.h"
#include "../common.h"
#include "../stringtools.h"

void LineOperator::Print(std::ostream& out) const
{
	out << "l='" << expr << "'";
}

char LineOperator::MyChar()
{
	return 'l';
}

static bool is_wild(char c)
{
	return (c=='*') || (c=='?');
}

void LineOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	expr = unparan(str);
	if (expr.empty() || !is_wild(expr.front()) || !is_wild(expr.back()))
		prio = 2;
	else
		prio = 15;
}

void LineOperator::MatchFile(File&, FileMatchStack& m)
{
	m.push_back(TriBool::Maybe);
}

void LineOperator::MatchLines(File& f, LineMatchStack& m)
{
	UnCache();
	LinesCache(f);
	ExeCached(m);
}

void LineOperator::LinesCache(File& f)
{
	if (have_cache) return;
	UL ln = 0;
	auto b = f.lines().begin();
	auto e = f.lines().end();
	if (expr.empty())
	{
		for (auto li = b; li!=e; ++li,++ln)
		{
			if (li->empty())
				lm_cache.addSimpleMatch(ln);
		}
	}
	else
	{
		bool backtest = !is_wild(expr.back());
		for (auto li = b; li!=e; ++li,++ln)
		{
			if (li->empty()) continue;
			if (backtest)
				if (expr.back() != li->back())
					continue;
			if (strPatMat(*li, expr))
			{
				auto vzzp = strPatMatSpecial(*li, expr);
				if (vzzp.empty())
					lm_cache.addSimpleMatch(ln);
				else
					for(auto&& zzp : vzzp)
						lm_cache.addFullMatch(ln, zzp.first, zzp.second);
			}
		}
	}
	have_cache = true;
}

bool LineOperator::IsCached() { return have_cache; }

int LineOperator::MyPrio() { return prio; }

void LineOperator::ExeCached(LineMatchStack& lms)
{
	if (have_cache)
		lms.push_back(lm_cache);
	else
		lms.push_back({TriBool::Maybe});
}

void LineOperator::UnCache()
{
	have_cache = false;
	lm_cache = LineMatch{false,{}};
}

// ----------------------------------------------------------------------------

void LineCIOperator::Print(std::ostream& out) const
{
	out << "L='" << expr << "'";
}

char LineCIOperator::MyChar()
{
	return 'L';
}

void LineCIOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	expr = to_lower_copy(unparan(str));
	if (expr.empty() || !is_wild(expr.front()) || !is_wild(expr.back()))
		prio = 3;
	else
		prio = 15;
}

void LineCIOperator::MatchFile(File&, FileMatchStack& m)
{
	m.push_back(TriBool::Maybe);
}

void LineCIOperator::LinesCache(File& f)
{
	if (have_cache) return;
	UL ln = 0;
	auto b = f.lines().begin();
	auto e = f.lines().end();
	if (expr.empty())
	{
		for (auto li = b; li!=e; ++li,++ln)
		{
			if (li->empty())
				lm_cache.addSimpleMatch(ln);
		}
	}
	else
	{
		bool backtest = !is_wild(expr.back());
		for (auto li = b; li!=e; ++li,++ln)
		{
			if (li->empty()) continue;
			if (backtest)
				if (expr.back() != std::tolower(li->back()))
					continue;
			auto lci = to_lower_copy(*li);
			if (strPatMat(lci, expr))
			{
				auto vzzp = strPatMatSpecial(lci, expr);
				if (vzzp.empty())
					lm_cache.addSimpleMatch(ln);
				else
					for(auto&& zzp : vzzp)
						lm_cache.addFullMatch(ln, zzp.first, zzp.second);
			}
		}
	}
	have_cache = true;
}

void LineCIOperator::MatchLines(File& f, LineMatchStack& m)
{
	UnCache();
	LinesCache(f);
	ExeCached(m);
}

bool LineCIOperator::IsCached() { return have_cache; }

int LineCIOperator::MyPrio() { return prio; }

void LineCIOperator::ExeCached(LineMatchStack& lms)
{
	if (have_cache)
		lms.push_back(lm_cache);
	else
		lms.push_back({TriBool::Maybe});
}

void LineCIOperator::UnCache()
{
	have_cache = false;
	lm_cache = LineMatch{false,{}};
}

