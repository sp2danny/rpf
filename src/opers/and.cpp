
#include "../operators.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void AndOperator::Print(std::ostream& out) const
{
	out << "a";
	if (cnt == 0)
		out << "+";
	else if (cnt > 1)
		out << cnt;
}

char AndOperator::MyChar()
{
	return 'a';
}

void AndOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() == 1)
		cnt = 1;
	else if (str=="a+")
		cnt = 0;
	else
		cnt = getParam(str, 1);
}

void AndOperator::MatchFile([[maybe_unused]] File& f, FileMatchStack& m)
{
	auto do_a_and = [&]() -> void
	{
		TriBool m1 = m.back(); m.pop_back();
		TriBool m2 = m.back(); m.pop_back();
		TriBool res = And(m1, m2);
		m.push_back(res);
	};

	if (cnt)
	{
		for (UL i = 0; i<cnt; ++i)
		{
			if (m.size() < 2)
				throw "operator and: not enough operands";
			do_a_and();
		}
	} else {
		while (m.size() >= 2)
		{
			do_a_and();
		}
	}
}

void AndOperator::MatchLines([[maybe_unused]] File& f, LineMatchStack& m)
{
	auto do_a_and = [&]() -> void
	{
		auto m2 = std::move(m.back()); m.pop_back();
		auto m1 = std::move(m.back()); m.pop_back();
		if (m1.match() && m2.match())
		{
			LineMatch res;
			res.match(true);
			res.modifiableLines() = std::move(m1.modifiableLines());
			for (auto&& l : m2.lines())
			{
				if (l.second.empty())
					res.addSimpleMatch(l.first);
				else
					for (auto&& x : l.second)
						res.addFullMatch(l.first, x);
			}
			m.push_back(std::move(res));
		} else {
			m.emplace_back(And(m1.tri(), m2.tri()));
		}
	};

	if (cnt)
	{
		for (UL i = 0; i<cnt; ++i)
		{
			if (m.size() < 2)
				throw "operator and: not enough operands";
			do_a_and();
		}
	} else {
		while (m.size() >= 2)
		{
			do_a_and();
		}
	}
}


