
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void TrueOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<TrueOperator>(TrueOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* TrueOperator::clone()
{
	return new TrueOperator(*this);
};

char TrueOperator::MyChar()
{
	return 't';
}

void TrueOperator::Create ( [[maybe_unused]] std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() != 1)
		throw "operator syntax error";
}

/*void TrueOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_true);
}*/

void TrueOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_true);
}

void TrueOperator::MatchLines ( File& , LineMatchStack& m )
{
	m.push_back({true,{}});
}

