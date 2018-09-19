
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void DirOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<DirOperator>(DirOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* DirOperator::clone()
{
	return new DirOperator(*this);
};

char DirOperator::MyChar()
{
	return 'd';
}

void DirOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	name = to_lower_copy(unparan(str));
}

void DirOperator::MatchDir ( File& f, FileMatchStack& m )
{
	if (str_pat_mat(to_lower_copy(f.path), name))
		m.push_back(tb_true);
	else
		m.push_back(tb_false);
}

void DirOperator::MatchFile ( File& f, FileMatchStack& m )
{
	return MatchDir(f, m);
}

void DirOperator::MatchLines ( File& f, LineMatchStack& m )
{
	if (str_pat_mat(to_lower_copy(f.path), name))
		m.emplace_back(true, Lines{});
	else
		m.emplace_back(false, Lines{});
}













































