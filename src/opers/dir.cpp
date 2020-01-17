
#include "../operators.h"
#include "../common.h"
#include "../stringtools.h"

char DirOperator::MyChar()
{
	return 'd';
}

void DirOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	name = to_lower_copy(unparan(str));
}

void DirOperator::MatchFile(File& f, FileMatchStack& m)
{
	if (str_pat_mat(to_lower_copy(f.path), name))
		m.push_back(tb_true);
	else
		m.push_back(tb_false);
}

void DirOperator::MatchLines(File& f, LineMatchStack& m)
{
	if (str_pat_mat(to_lower_copy(f.path), name))
		m.emplace_back(true, Lines{});
	else
		m.emplace_back(false, Lines{});
}
