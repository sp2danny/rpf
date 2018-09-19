
#include "common.h"

bool LineMatch::match() const
{
	return m_match;
}

void LineMatch::match(bool m)
{
	m_match = m;
	if (!m)
		m_lines.clear();
}

const Lines& LineMatch::lines() const
{
	return m_lines;
}

Lines& LineMatch::modifiable_lines()
{
	return m_lines;
}

void LineMatch::add_simple_match(UL l)
{
	m_match = true;
	m_lines[l];
}

void LineMatch::add_full_match(UL l, UL b, UL e)
{
	m_match = true;
	m_lines[l].push_back({b,e});
}

void LineMatch::add_full_match(UL l, MIP mip)
{
	m_match = true;
	m_lines[l].push_back(mip);
}

void LineMatch::add_full_match(LIter li, UL b, UL e)
{
	m_match = true;
	li->second.push_back({b,e});
}

void LineMatch::add_full_match(LIter li, MIP mip)
{
	m_match = true;
	li->second.push_back(mip);
}

bool LineMatch::have_line(UL l) const
{
	return m_lines.count(l) != 0;
}

bool LineMatch::have_char(LCIter li, UL idx) const
{
	for (auto&& mip : li->second)
	{
		if ((idx >= mip.first) && (idx < mip.second))
			return true;
	}
	return false;
}

bool LineMatch::have_char(UL l, UL idx) const
{
	auto itr = m_lines.find(l);
	if (itr == m_lines.end())
		return false;
	else
		return have_char(itr, idx);
}
