
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void NearOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<NearOperator>(NearOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* NearOperator::clone()
{
	return new NearOperator(*this);
};

char NearOperator::MyChar()
{
	return 'n';
}

void NearOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	n = getparam(str, 5);
}

/*void NearOperator::MatchDir ( File&, FileMatchStack& m )
{
	if (m.size() < 2)
	{
		throw "operator near: not enough operands";
	}
	TriBool m1 = m.back(); m.pop_back();
	TriBool m2 = m.back(); m.pop_back();
	TriBool res = And(m1, m2);
	m.push_back(res);
}*/

void NearOperator::MatchFile ( [[maybe_unused]] File& f, FileMatchStack& m )
{
	if (m.size() < 2)
	{
		throw "operator near: not enough operands";
	}
	TriBool m1 = m.back(); m.pop_back();
	TriBool m2 = m.back(); m.pop_back();
	TriBool res = And(m1, m2);
	m.push_back(res);
}

void NearOperator::MatchLines ( File& , LineMatchStack& m )
{
	if (m.size() < 2)
	{
		throw "operator near: not enough operands";
	}

	auto m2 = std::move(m.back()); m.pop_back();
	auto m1 = std::move(m.back()); m.pop_back();

	if ((!m1.match()) || (!m2.match()))
	{
		m.emplace_back();
		return;
	}

	LineMatch res;
	for (auto&& l1 : m1.lines())
	{
		for(auto&& l2 : m2.lines())
		{
			auto diff = (l2.first>l1.first) ? l2.first-l1.first : l1.first-l2.first;
			if (diff <= n)
			{
				res.add_simple_match(l1.first);
				res.add_simple_match(l2.first);
				for (auto&& x : l1.second)
					res.add_full_match(l1.first, x);
				for (auto&& x : l2.second)
					res.add_full_match(l2.first, x);
			}
		}
	}
	m.push_back(std::move(res));
}
