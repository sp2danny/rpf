
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void OrOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<OrOperator>(OrOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* OrOperator::clone()
{
	return new OrOperator(*this);
};

char OrOperator::MyChar()
{
	return 'o';
}

void OrOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() != 1)
		throw "operator syntax error";
}

// enum TriBool { tb_false, tb_true, tb_maybe };

void OrOperator::MatchDir ( File&, FileMatchStack& m )
{
	if (m.size() < 2)
	{
		throw "operator or: not enough operands";
	}
	TriBool m1 = m.back(); m.pop_back();
	TriBool m2 = m.back(); m.pop_back();
	TriBool res = Or(m1, m2);
	m.push_back(res);
}

void OrOperator::MatchFile ( File& f, FileMatchStack& m )
{
	MatchDir(f, m);
}

void OrOperator::MatchLines ( File& , LineMatchStack& m )
{
	if (m.size() < 2)
	{
		throw "operator or: not enough operands";
	}
	auto m2 = std::move(m.back()); m.pop_back();
	auto m1 = std::move(m.back()); m.pop_back();
	if (m1.match() || m2.match())
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







