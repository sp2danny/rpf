
#include "../operators.h"
#include "../common.h"

void SubOperator::Print(std::ostream& out) const
{
	out << "-";
}

char SubOperator::MyChar()
{
	return '-';
}

void SubOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() != 1)
		throw "operator syntax error";
}

void SubOperator::MatchFile([[maybe_unused]] File& f, FileMatchStack& m)
{
	if (m.size() < 2)
	{
		throw "operator sub: not enough operands";
	}
	m.pop_back();
}

void SubOperator::MatchLines(File&, LineMatchStack& m)
{
	if (m.size() < 2)
	{
		throw "operator sub: not enough operands";
	}
	auto m2 = std::move(m.back()); m.pop_back();
	auto m1 = std::move(m.back()); m.pop_back();

	if (m1.lines().empty())
	{
		m.push_back(m1);
		return;
	}

	LineMatch res{true, {}};
	res.modifiableLines() = move(m1.lines());
	for (auto&& l : m2.lines())
	{
		res.modifiableLines().erase(l.first);
	}
	res.match(!res.lines().empty());
	m.push_back(std::move(res));
}
