

#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void BMOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<BMOperator>(BMOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* BMOperator::clone()
{
	return new BMOperator(*this);
};

char BMOperator::MyChar()
{
	return 'b';
}

void BMOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	str = unparan(str);
	sz = str.size();
	bm.emplace(str);
}

/* void BMOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
} */

void BMOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void BMOperator::MatchLines ( File& f, LineMatchStack& m )
{
	LineMatch res;
	UL ln = 0;
	for (auto&& l : f.lines())
	{
		auto lst = bm->match_all(l);
		for (auto&& p : lst)
			res.add_full_match(ln, p, p+sz);
		++ln;
	}
	m.push_back(std::move(res));
}

// ----------------------------------------------------------------------------

void BM_CIOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<BM_CIOperator>(BM_CIOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* BM_CIOperator::clone()
{
	return new BM_CIOperator(*this);
};

char BM_CIOperator::MyChar()
{
	return 'B';
}

void BM_CIOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	str = unparan(str);
	sz = str.size();
	bm.emplace(str);
}

/* void BM_CIOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
} */

void BM_CIOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void BM_CIOperator::MatchLines ( File& f, LineMatchStack& m )
{
	LineMatch res;
	UL ln = 0;
	for (auto&& l : f.lines())
	{
		auto lst = bm->match_all(l);
		for (auto&& p : lst)
			res.add_full_match(ln, p, p+sz);
		++ln;
	}
	m.push_back(std::move(res));
}
