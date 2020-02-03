
#include "../operators.h"
#include "../common.h"
#include "../stringtools.h"

void DirOperator::Print(std::ostream& out) const
{
	out << "b='" << name << "'";
}

char DirOperator::MyChar()
{
	return 'd';
}

void DirOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	name = to_lower_copy(unParan(str));
}

void DirOperator::MatchFile(File& f, FileMatchStack& m)
{
	if (strPatMat(to_lower_copy(f.path), name))
		m.push_back(TriBool::True);
	else
		m.push_back(TriBool::False);
}

void DirOperator::MatchLines(File& f, LineMatchStack& m)
{
	if (strPatMat(to_lower_copy(f.path), name))
		m.emplace_back(true, Lines{});
	else
		m.emplace_back(false, Lines{});
}
