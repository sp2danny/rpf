
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void LineOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<LineOperator>(LineOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* LineOperator::clone()
{
	return new LineOperator(*this);
};

char LineOperator::MyChar()
{
	return 'l';
}

void LineOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	expr = unparan(str);
}

void LineOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void LineOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void LineOperator::MatchLines ( File& f, LineMatchStack& m )
{
	LineMatch res;
	UL ln = 0;
	for (auto&& l : f.lines())
	{
		if (str_pat_mat(l, expr))
		{
			auto vzzp = str_pat_mat_special(l, expr);
			if (vzzp.empty())
				res.add_simple_match(ln);
			else
				for(auto&& zzp : vzzp)
					res.add_full_match(ln, zzp.first, zzp.second);
		}
		++ln;
	}
	m.push_back(std::move(res));
}




































