
#include "../operators.h"
#include "../common.h"

char RegexOperator::MyChar()
{
	return 'r';
}

void RegexOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	str = unparan(str);
	re.emplace(str);
}

void RegexOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void RegexOperator::MatchLines ( File& f, LineMatchStack& m )
{
	UnCache();
	LinesCache(f);
	ExeCached(m);
}

void RegexOperator::LinesCache(File& f)
{
	if (have_cache) return;
	UL ln = 0;
	typedef std::string::const_iterator SCI;
	for (auto&& l : f.lines())
	{
		SCI lb = l.cbegin();
		SCI le = l.cend();
		std::smatch sm;
		SCI iter = lb;
		while (regex_search(iter, le, sm, *re))
		{
			if (sm[0].matched)
			{
				SCI beg = sm[0].first;
				SCI end = sm[0].second;
				lm_cache.add_full_match(ln, beg-lb, end-lb);
				iter = std::next(beg);
			}
			else
				break;
		}
		++ln;
	}
	have_cache = true;
}

bool RegexOperator::IsCached() { return have_cache; }

int RegexOperator::MyPrio() { return 5; }

void RegexOperator::ExeCached(LineMatchStack& lms)
{
	if (have_cache)
		lms.push_back(lm_cache);
	else
		lms.push_back({tb_maybe});
}

void RegexOperator::UnCache()
{
	have_cache = false;
	lm_cache = LineMatch{false,{}};
}

