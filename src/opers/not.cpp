
#include "../operators.h"
#include "../common.h"

void NotOperator::Print(std::ostream& out) const
{
	out << "!";
}

char NotOperator::MyChar()
{
	return '!';
}

void NotOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() != 1)
		throw "operator syntax error";
}

void NotOperator::MatchFile ( [[maybe_unused]] File& f, FileMatchStack& m )
{
	if (m.size() < 1)
	{
		throw "operator not: not enough operands";
	}
	TriBool m1 = m.back(); m.pop_back();
	TriBool res = Not(m1);
	m.push_back(res);
}

void NotOperator::MatchLines ( File& , LineMatchStack& m )
{
	if (m.size() < 1)
	{
		throw "operator not: not enough operands";
	}
	auto& m1 = m.back();
	m1 = LineMatch{Not(m1.tri())};
	m1.modifiable_lines().clear();
}
