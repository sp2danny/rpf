
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
	expr = unParan(str);
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
	if (mHaveCache) return;
	UL ln = 0;
	auto b = f.lines().begin();
	auto e = f.lines().end();
	if (expr.empty())
	{
		for (auto li = b; li!=e; ++li,++ln)
		{
			if (li->empty())
				mLmCache.addSimpleMatch(ln);
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
					mLmCache.addSimpleMatch(ln);
				else
					for(auto&& zzp : vzzp)
						mLmCache.addFullMatch(ln, zzp.first, zzp.second);
			}
		}
	}
	mHaveCache = true;
}

bool LineOperator::IsCached() { return mHaveCache; }

int LineOperator::MyPrio() { return prio; }

void LineOperator::ExeCached(LineMatchStack& lms)
{
	if (mHaveCache)
		lms.push_back(mLmCache);
	else
		lms.push_back({TriBool::Maybe});
}

void LineOperator::UnCache()
{
	mHaveCache = false;
	mLmCache = LineMatch{false,{}};
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
	expr = to_lower_copy(unParan(str));
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
	if (mHaveCache) return;
	UL ln = 0;
	auto b = f.lines().begin();
	auto e = f.lines().end();
	if (expr.empty())
	{
		for (auto li = b; li!=e; ++li,++ln)
		{
			if (li->empty())
				mLmCache.addSimpleMatch(ln);
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
					mLmCache.addSimpleMatch(ln);
				else
					for(auto&& zzp : vzzp)
						mLmCache.addFullMatch(ln, zzp.first, zzp.second);
			}
		}
	}
	mHaveCache = true;
}

void LineCIOperator::MatchLines(File& f, LineMatchStack& m)
{
	UnCache();
	LinesCache(f);
	ExeCached(m);
}

bool LineCIOperator::IsCached() { return mHaveCache; }

int LineCIOperator::MyPrio() { return prio; }

void LineCIOperator::ExeCached(LineMatchStack& lms)
{
	if (mHaveCache)
		lms.push_back(mLmCache);
	else
		lms.push_back({TriBool::Maybe});
}

void LineCIOperator::UnCache()
{
	mHaveCache = false;
	mLmCache = LineMatch{false,{}};
}

