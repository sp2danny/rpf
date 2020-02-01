
#include "common.h"
#include "OFM.h"

bool LineMatch::match() const
{
	return m_match == TriBool::True;
}

void LineMatch::match(bool m)
{
	m_match = FromBool(m);
	if (!m)
		m_lines.clear();
}

const Lines& LineMatch::lines() const
{
	return m_lines;
}

Lines& LineMatch::modifiableLines()
{
	return m_lines;
}

void LineMatch::addSimpleMatch(UL l)
{
	m_match = TriBool::True;
	m_lines[l];
}

void LineMatch::addFullMatch(UL l, UL b, UL e)
{
	m_match = TriBool::True;
	m_lines[l].push_back({b,e});
}

void LineMatch::addFullMatch(UL l, MIP mip)
{
	m_match = TriBool::True;
	m_lines[l].push_back(mip);
}

void LineMatch::addFullMatch(LIter li, UL b, UL e)
{
	m_match = TriBool::True;
	li->second.push_back({b,e});
}

void LineMatch::addFullMatch(LIter li, MIP mip)
{
	m_match = TriBool::True;
	li->second.push_back(mip);
}

void LineMatch::addAllMatches(UL l, const MMIP& mmip)
{
	m_match = TriBool::True;
	for (const auto& mip : mmip)
		m_lines[l].push_back(mip);
}


bool LineMatch::haveLine(UL l) const
{
	return m_lines.count(l) != 0;
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
	auto itr = m_lines.find(l);
	if (itr == m_lines.end())
		return false;
	else
		return haveChar(itr, idx);
}
