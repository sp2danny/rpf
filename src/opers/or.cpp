
#include "../operators.h"
#include "../common.h"

void OrOperator::Print(std::ostream& out) const
{
	out << "o";
	if (cnt == 0)
		out << "+";
	else if (cnt > 1)
		out << cnt;
}

char OrOperator::MyChar()
{
	return 'o';
}

void OrOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() == 1)
		cnt = 1;
	else if (str=="o+")
		cnt = 0;
	else
		cnt = getParam(str, 1);
	//std::cerr << "or using " << cnt << " cnt\n";
}

void OrOperator::MatchFile([[maybe_unused]] File& f, FileMatchStack& m)
{
	auto doAnOr = [&]() -> void
	{
		TriBool m1 = m.back(); m.pop_back();
		TriBool m2 = m.back(); m.pop_back();
		TriBool res = Or(m1, m2);
		m.push_back(res);
	};

	if (cnt)
	{
		for (UL i = 0; i<cnt; ++i)
		{
			if (m.size() < 2)
			{
				//printStack(m);
				throw "operator or: not enough operands";
			}
			doAnOr();
		}
	} else {
		while (m.size() >= 2)
		{
			doAnOr();
		}
	}
}

void OrOperator::MatchLines([[maybe_unused]] File& f, LineMatchStack& m)
{
	auto doAnOr = [&]() -> void
	{
		auto m2 = std::move(m.back()); m.pop_back();
		auto m1 = std::move(m.back()); m.pop_back();
		if (m1.match() || m2.match())
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
			m.emplace_back( Or(m1.tri(), m2.tri()) );
		}
	};

	if (cnt)
	{
		for (UL i = 0; i<cnt; ++i)
		{
			if (m.size() < 2)
			{
				//printStack(m);
				throw "operator or: not enough operands";
			}
			doAnOr();
		}
	} else {
		while (m.size() >= 2)
		{
			doAnOr();
		}
	}
}


