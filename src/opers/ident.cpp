
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

char IdentOperator::MyChar()
{
	return 'i';
}

void IdentOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	id = unparan(str);
}

void IdentOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void IdentOperator::MatchLines ( File& f, LineMatchStack& m )
{
	LineMatch res;
	UL ln = 0;
	UL sz = id.size();
	for (auto&& l : f.lines())
	{
		auto vsp = tokenify(l);
		for (auto&& t : vsp)
		{
			if (t.first == id)
			{
				res.add_full_match(ln, t.second, t.second+sz);
			}
		}
		++ln;
	}
	m.push_back(std::move(res));
}

// ----------------------------------------------------------------------------

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
	LineMatch res;
	UL ln = 0;
	UL sz = id.size();
	for (auto&& l : f.lines())
	{
		auto vsp = tokenify(l);
		for (auto&& t : vsp)
		{
			if (to_lower_copy(t.first) == id)
			{
				res.add_full_match(ln, t.second, t.second+sz);
			}
		}
		++ln;
	}
	m.push_back(std::move(res));
}
