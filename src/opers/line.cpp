
#include "../operators.h"

void LineOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<LineOperator>(LineOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

char LineOperator::MyChar()
{
	return 'f';
}

void LineOperator::Create ( std::string str )
{
}

void LineOperator::MatchDir ( File&, FileMatchStack )
{
}

void LineOperator::MatchFile ( File&, FileMatchStack )
{
}

void LineOperator::MatchLines ( File&, LineMatchStack )
{
}


