
#include "src/operators.h"
#include "src/common.h"

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
	expr = to_lower_copy(unparan(str));
}

void LineOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void LineOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void LineOperator::MatchLines ( File& , LineMatchStack& m )
{
	m.push_back({false, {}});
}




