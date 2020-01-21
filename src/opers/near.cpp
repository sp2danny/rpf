
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void NearOperator::Print(std::ostream& out) const
{
	out << "n";
	if (all)
		out << "+";
	else
		if (n > 1)
			out << n;
}

char NearOperator::MyChar()
{
	return 'n';
}

void NearOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str == "n+")
	{
		n = 0; all = true;
	}
	else
		n = getparam(str, 5);
}

void NearOperator::MatchFile ( [[maybe_unused]] File& f, FileMatchStack& m )
{
	UL sz = m.size();

	if (all)
	{
		if (sz < 1) throw "operator near: not enough operands";
		while (sz >= 2)
		{
			TriBool m1 = m.back(); m.pop_back();
			TriBool m2 = m.back(); m.pop_back();
			TriBool res = And(m1, m2);
			m.push_back(res);
			--sz;
		}
	}
	else
	{
		if (sz < 2) throw "operator near: not enough operands";
		TriBool m1 = m.back(); m.pop_back();
		TriBool m2 = m.back(); m.pop_back();
		TriBool res = And(m1, m2);
		m.push_back(res);
	}
}

void NearOperator::MatchLines ( File& , LineMatchStack& m )
{

	auto near = [](const LineMatch& m1, const LineMatch& m2, UL n) -> LineMatch
	{
		LineMatch res;
		if (m1.match() && m2.match())
		{
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
			res.match(!res.lines().empty());
		}
		else
		{
			res = LineMatch{And(m1.tri(), m2.tri())};
		}
		return res;
	};

	UL sz = m.size();
	if (all)
	{
		if (sz < 1) throw "operator near: not enough operands";
		while (sz >= 2)
		{
			auto m2 = std::move(m.back()); m.pop_back();
			auto m1 = std::move(m.back()); m.pop_back();
			auto tb1 = m1.tri();
			auto tb2 = m2.tri();
			/**/ if ((tb1==tb_false) || (tb2==tb_false))
				m.push_back({tb_false});
			else if ((tb1==tb_maybe) || (tb2==tb_maybe))
				m.push_back({tb_maybe});
			else if (m1.lines().empty())
				m.push_back(std::move(m2));
			else if (m2.lines().empty())
				m.push_back(std::move(m1));
			else
				m.push_back(near(m1, m2, n));
			--sz;
		}
	}
	else
	{
		if (sz < 2) throw "operator near: not enough operands";
		auto m2 = std::move(m.back()); m.pop_back();
		auto m1 = std::move(m.back()); m.pop_back();
		m.push_back(near(m1, m2, n));
	}
}


