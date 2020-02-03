
#include "../operators.h"
#include "../common.h"

void RegexOperator::Print(std::ostream& out) const
{
	out << "r='" << re->mStr << "'";
}

char RegexOperator::MyChar()
{
	return 'r';
}

void RegexOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	str = unParan(str);
	re.emplace(str);
}

void RegexOperator::MatchFile(File&, FileMatchStack& m)
{
	m.push_back(TriBool::Maybe);
}

void RegexOperator::MatchLines(File& f, LineMatchStack& m)
{
	UnCache();
	LinesCache(f);
	ExeCached(m);
}

void RegexOperator::LinesCache(File& f)
{
	if (mHaveCache) return;
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
				mLmCache.addFullMatch(ln, beg-lb, end-lb);
				iter = std::next(beg);
			}
			else
				break;
		}
		++ln;
	}
	mHaveCache = true;
}

bool RegexOperator::IsCached() { return mHaveCache; }

int RegexOperator::MyPrio() { return 5; }

void RegexOperator::ExeCached(LineMatchStack& lms)
{
	if (mHaveCache)
		lms.push_back(mLmCache);
	else
		lms.push_back({TriBool::Maybe});
}

void RegexOperator::UnCache()
{
	mHaveCache = false;
	mLmCache = LineMatch{false,{}};
}

