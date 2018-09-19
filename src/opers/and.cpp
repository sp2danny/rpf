
#include "src/operators.h"
#include "src/common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void AndOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<AndOperator>(AndOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* AndOperator::clone()
{
	return new AndOperator(*this);
};

char AndOperator::MyChar()
{
	return 'a';
}

void AndOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() != 1)
		throw "operator syntax error";
}

void AndOperator::MatchDir ( File&, FileMatchStack& m )
{
	if (m.size() < 2)
	{
		throw "operator and: not enough operands";
	}
	TriBool m1 = m.back(); m.pop_back();
	TriBool m2 = m.back(); m.pop_back();
	TriBool res;
	switch (m1)
	{
	case tb_false:
		res = tb_false;
		break;
	case tb_maybe:
		res = (m2==tb_false) ? tb_false : tb_maybe;
		break;
	case tb_true:
		res = m2;
		break;
	}
	m.push_back(res);
}

void AndOperator::MatchFile ( File& f, FileMatchStack& m )
{
	MatchDir(f, m);
}

void AndOperator::MatchLines ( File& , LineMatchStack& m )
{
	auto m2 = std::move(m.back()); m.pop_back();
	auto m1 = std::move(m.back()); m.pop_back();
	if (m1.match() && m2.match())
	{
		LineMatch res;
		res.match(true);
		res.modifiable_lines() = std::move(m1.modifiable_lines());
		for (auto&& l : m2.lines())
		{
			if (l.second.empty())
				res.add_simple_match(l.first);
			else
				for (auto&& x : l.second)
					res.add_full_match(l.first, x);
		}
		m.push_back(std::move(res));
	} else {
		m.emplace_back();
	}
}













