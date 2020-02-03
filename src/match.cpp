
#include "common.h"
#include "OFM.h"

bool LineMatch::match() const
{
	return mMatch == TriBool::True;
}

void LineMatch::match(bool m)
{
	mMatch = FromBool(m);
	if (!m)
		mLines.clear();
}

const Lines& LineMatch::lines() const
{
	return mLines;
}

Lines& LineMatch::modifiableLines()
{
	return mLines;
}

void LineMatch::addSimpleMatch(UL l)
{
	mMatch = TriBool::True;
	mLines[l];
}

void LineMatch::addFullMatch(UL l, UL b, UL e)
{
	mMatch = TriBool::True;
	mLines[l].push_back({b,e});
}

void LineMatch::addFullMatch(UL l, MIP mip)
{
	mMatch = TriBool::True;
	mLines[l].push_back(mip);
}

void LineMatch::addFullMatch(LIter li, UL b, UL e)
{
	mMatch = TriBool::True;
	li->second.push_back({b,e});
}

void LineMatch::addFullMatch(LIter li, MIP mip)
{
	mMatch = TriBool::True;
	li->second.push_back(mip);
}

void LineMatch::addAllMatches(UL l, const MMIP& mmip)
{
	mMatch = TriBool::True;
	for (const auto& mip : mmip)
		mLines[l].push_back(mip);
}


bool LineMatch::haveLine(UL l) const
{
	return mLines.count(l) != 0;
}

bool LineMatch::haveChar(LCIter li, UL idx) const
{
	for (auto&& mip : li->second)
	{
		if ((idx >= mip.first) && (idx < mip.second))
			return true;
	}
	return false;
}

bool LineMatch::haveChar(UL l, UL idx) const
{
	auto itr = mLines.find(l);
	if (itr == mLines.end())
		return false;
	else
		return haveChar(itr, idx);
}
