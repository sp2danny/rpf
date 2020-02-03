
#include "../operators.h"
#include "../common.h"

void RangeOperator::Print(std::ostream& out) const
{
	out << "=" << max;
}

char RangeOperator::MyChar()
{
	return '=';
}

void RangeOperator::Create([[maybe_unused]] std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	max = getParam(str, 5);
}

void RangeOperator::MatchFile([[maybe_unused]] File& f, FileMatchStack& m)
{
	UL sz = m.size();

	if (sz < 2) throw "operator range: not enough operands";
	TriBool m1 = m.back(); m.pop_back();
	TriBool m2 = m.back(); m.pop_back();
	TriBool res = And(m1, m2);
	m.push_back(res);
}

void RangeOperator::MatchLines(File&, LineMatchStack& m)
{
	UL sz = m.size();
	if (sz < 2) throw "operator range: not enough operands";
	
	auto m2 = std::move(m.back()); m.pop_back();
	auto m1 = std::move(m.back()); m.pop_back();

	LineMatch res;
	if (m1.match() && m2.match())
	{
		auto i1 = m1.lines().begin();
		auto e1 = m1.lines().end();
		while (i1 != e1)
		{
			auto i2 = m2.lines().begin();
			auto e2 = m2.lines().end();
			while (i2 != e2)
			{
				auto ln1 = i1->first;
				auto ln2 = i2->first;
				if (ln2 >= ln1)
				{
					while (true)
					{
						auto ii = i1; ++ii;
						if (ii == e1) break;
						if (ii->first <= ln2)
						{
							i1 = ii;
							ln1 = i1->first;
							continue;
						}
						else
						{
							break;
						}
					}
					if ((ln2 - ln1) <= max)
					{
						res.addAllMatches(ln1, i1->second);
						res.addAllMatches(ln2, i2->second);
						for (auto ln = ln1; ln <= ln2; ++ln)
							res.addSimpleMatch(ln);
					}
					break;
				}
				else
				{
					++i2;
				}
			}
			++i1;
		}
		res.match(!res.lines().empty());
	}
	else
	{
		res = LineMatch{And(m1.tri(), m2.tri())};
	}
	m.push_back(std::move(res));
}


