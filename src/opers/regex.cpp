

#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void RegexOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<RegexOperator>(RegexOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* RegexOperator::clone()
{
	return new RegexOperator(*this);
};

char RegexOperator::MyChar()
{
	return 'r';
}

void RegexOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	str = unparan(str);
	re.emplace(str);
}

void RegexOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void RegexOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void RegexOperator::MatchLines ( File& f, LineMatchStack& m )
{
	LineMatch res;
	UL ln = 0;
	typedef std::string::const_iterator SCI;
	for (auto&& l : f.lines())
	{
		SCI lb = l.cbegin();
		SCI le = l.cend();
		std::smatch sm;
		while (regex_search(lb, le, sm, *re))
		{
			if (sm[0].matched)
			{
				SCI beg = sm[0].first;
				SCI end = sm[0].second;
				res.add_full_match(ln, beg-lb, end-lb);
				lb = std::next(beg);
			}
			else
				break;
		}
		++ln;
	}
	m.push_back(std::move(res));
}
