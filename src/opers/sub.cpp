
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void SubOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<SubOperator>(SubOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* SubOperator::clone()
{
	return new SubOperator(*this);
};

char SubOperator::MyChar()
{
	return '-';
}

void SubOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() != 1)
		throw "operator syntax error";
}

/*void SubOperator::MatchDir ( File&, FileMatchStack& m )
{
	if (m.size() < 2)
	{
		throw "operator sub: not enough operands";
	}
	m.pop_back();
}*/

void SubOperator::MatchFile ( [[maybe_unused]] File& f, FileMatchStack& m )
{
	if (m.size() < 2)
	{
		throw "operator sub: not enough operands";
	}
	m.pop_back();
}

void SubOperator::MatchLines ( File&, LineMatchStack& m )
{
	if (m.size() < 2)
	{
		throw "operator sub: not enough operands";
	}
	auto m2 = std::move(m.back()); m.pop_back();
	auto m1 = std::move(m.back()); m.pop_back();

	if (m1.lines().empty() || !m1.match())
	{
		m.push_back({false,{}});
		return;
	}

	LineMatch res{true, {}};
	res.modifiable_lines() = move(m1.lines());
	for (auto&& l : m2.lines())
	{
		res.modifiable_lines().erase(l.first);
	}
	res.match(!res.lines().empty());
	m.push_back(std::move(res));
}
