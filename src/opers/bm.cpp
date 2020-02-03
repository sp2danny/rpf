
#include "../operators.h"
#include "../common.h"
#include "../stringtools.h"

void BMOperator::Print(std::ostream& out) const
{
	out << "b='" << bm->getText() << "'";
}

char BMOperator::MyChar()
{
	return 'b';
}

void BMOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	str = unParan(str);
	sz = str.size();
	bm.emplace(str);
}

void BMOperator::MatchFile(File&, FileMatchStack& m)
{
	m.push_back(TriBool::Maybe);
}

void BMOperator::MatchLines(File& f, LineMatchStack& m)
{
	UnCache();
	LinesCache(f);
	ExeCached(m);
}

void BMOperator::LinesCache(File& f)
{
	if (mHaveCache) return;
	UL ln = 0;
	for (auto&& l : f.lines())
	{
		auto lst = bm->matchAll(l);
		for (auto&& p : lst)
			mLmCache.addFullMatch(ln, p, p+sz);
		++ln;
	}
	mHaveCache = true;
}

bool BMOperator::IsCached() { return mHaveCache; }

int BMOperator::MyPrio() { return 3; }

void BMOperator::ExeCached(LineMatchStack& lms)
{
	if (mHaveCache)
		lms.push_back(mLmCache);
	else
		lms.push_back({TriBool::Maybe});
}

void BMOperator::UnCache()
{
	mHaveCache = false;
	mLmCache = LineMatch{false, {}};
}

// ----------------------------------------------------------------------------

void BM_CIOperator::Print(std::ostream& out) const
{
	out << "B='" << bm->getText() << "'";
}

char BM_CIOperator::MyChar()
{
	return 'B';
}

void BM_CIOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	str = unParan(str);
	sz = str.size();
	bm.emplace(str);
}

void BM_CIOperator::MatchFile(File&, FileMatchStack& m)
{
	m.push_back(TriBool::Maybe);
}

void BM_CIOperator::MatchLines(File& f, LineMatchStack& m)
{
	UnCache();
	LinesCache(f);
	ExeCached(m);
}

void BM_CIOperator::LinesCache(File& f)
{
	if (mHaveCache) return;
	UL ln = 0;
	for (auto&& l : f.lines())
	{
		auto lst = bm->matchAll(l);
		for (auto&& p : lst)
			mLmCache.addFullMatch(ln, p, p+sz);
		++ln;
	}
	mHaveCache = true;
}

bool BM_CIOperator::IsCached() { return mHaveCache; }

int BM_CIOperator::MyPrio() { return 4; }

void BM_CIOperator::ExeCached(LineMatchStack& lms)
{
	if (mHaveCache)
		lms.push_back(mLmCache);
	else
		lms.push_back({TriBool::Maybe});
}

void BM_CIOperator::UnCache()
{
	mHaveCache = false;
	mLmCache = LineMatch{false, {}};
}

