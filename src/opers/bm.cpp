

#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

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
