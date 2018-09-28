
#include "../operators.h"
#include "../common.h"

char RangeOperator::MyChar()
{
	return '=';
}

void RangeOperator::Create ( [[maybe_unused]] std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
}

void RangeOperator::MatchFile ( [[maybe_unused]] File& f, FileMatchStack& m )
{
	UL sz = m.size();

	if (sz < 2) throw "operator range: not enough operands";
	TriBool m1 = m.back(); m.pop_back();
	TriBool m2 = m.back(); m.pop_back();
	TriBool res = And(m1, m2);
	m.push_back(res);
}

void RangeOperator::MatchLines ( File& , LineMatchStack& m )
{
	UL sz = m.size();
	if (sz < 2) throw "operator range: not enough operands";
	
	auto m2 = std::move(m.back()); m.pop_back();
	auto m1 = std::move(m.back()); m.pop_back();

	LineMatch res;
	if (m1.match() && m2.match())
	{
		for (auto&& l1 : m1.lines())
		{
			for(auto&& l2 : m2.lines())
			{
				auto ln1 = l1.first;
				auto ln2 = l2.first;
				if (ln2 >= ln1)
				{
					res.add_all_matches(ln1, l1.second);
					res.add_all_matches(ln2, l2.second);
					for (auto ln = ln1; ln <= ln2; ++ln)
						res.add_simple_match(ln);
					continue;
				}
			}
		}
		res.match(!res.lines().empty());
	}
	else
	{
		res = LineMatch{And(m1.tri(), m2.tri())};
	}
	m.push_back(std::move(res));
}


