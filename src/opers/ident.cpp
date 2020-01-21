
#include "../operators.h"
#include "../common.h"
#include "../stringtools.h"

void IdentOperator::Print(std::ostream& out) const
{
	out << "i='" << id << "'";
}

char IdentOperator::MyChar()
{
	return 'i';
}

void IdentOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	id = unparan(str);
}

void IdentOperator::MatchFile(File&, FileMatchStack& m)
{
	m.push_back(tb_maybe);
}

void IdentOperator::MatchLines(File& f, LineMatchStack& m)
{
	UnCache();
	LinesCache(f);
	ExeCached(m);
}

void IdentOperator::LinesCache(File& f)
{
	if (have_cache) return;
	UL ln = 0;
	UL sz = id.size();
	for (auto&& l : f.lines())
	{
		auto vsp = tokenify(l);
		for (auto&& t : vsp)
		{
			if (t.first == id)
			{
				lm_cache.add_full_match(ln, t.second, t.second+sz);
			}
		}
		++ln;
	}
	have_cache = true;
}

bool IdentOperator::IsCached() { return have_cache; }

int IdentOperator::MyPrio() { return 20; }

void IdentOperator::ExeCached(LineMatchStack& lms)
{
	if (have_cache)
		lms.push_back(lm_cache);
	else
		lms.push_back({tb_maybe});
}

void IdentOperator::UnCache()
{
	have_cache = false;
	lm_cache = LineMatch{false,{}};
}

// ----------------------------------------------------------------------------

void IdentCIOperator::Print(std::ostream& out) const
{
	out << "I='" << id << "'";
}

char IdentCIOperator::MyChar()
{
	return 'I';
}

void IdentCIOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	id = to_lower_copy(unparan(str));
}

void IdentCIOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void IdentCIOperator::MatchLines ( File& f, LineMatchStack& m )
{
	UnCache();
	LinesCache(f);
	ExeCached(m);
}

void IdentCIOperator::LinesCache(File& f)
{
	if (have_cache) return;
	UL ln = 0;
	UL sz = id.size();
	for (auto&& l : f.lines())
	{
		auto vsp = tokenify(l);
		for (auto&& t : vsp)
		{
			if (to_lower_copy(t.first) == id)
			{
				lm_cache.add_full_match(ln, t.second, t.second+sz);
			}
		}
		++ln;
	}
	have_cache = true;
}

bool IdentCIOperator::IsCached() { return have_cache; }

int IdentCIOperator::MyPrio() { return 25; }

void IdentCIOperator::ExeCached(LineMatchStack& lms)
{
	if (have_cache)
		lms.push_back(lm_cache);
	else
		lms.push_back({tb_maybe});
}

void IdentCIOperator::UnCache()
{
	have_cache = false;
	lm_cache = LineMatch{false,{}};
}


