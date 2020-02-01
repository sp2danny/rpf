
#include "../operators.h"
#include "../common.h"
#include "../stringtools.h"

void FileOperator::Print(std::ostream& out) const
{
	out << "f='" << name << "'";
}

char FileOperator::MyChar()
{
	return 'f';
}

void FileOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	name = to_lower_copy(unparan(str));
}

void FileOperator::MatchFile(File& f, FileMatchStack& m)
{
	if (str_pat_mat(to_lower_copy(f.name), name))
		m.push_back(TriBool::True);
	else
		m.push_back(TriBool::False);
}

void FileOperator::MatchLines(File& f, LineMatchStack& m)
{
	if (str_pat_mat(to_lower_copy(f.name), name))
		m.emplace_back(true, Lines{});
	else
		m.emplace_back(false, Lines{});
}

// ----------------------------------------------------------------------------

void CppOnlyOperator::Print(std::ostream& out) const
{
	out << "+='" << name << "'";
}

char CppOnlyOperator::MyChar()
{
	return '+';
}

void CppOnlyOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	name = to_lower_copy(unparan(str));
}

void CppOnlyOperator::MatchFile(File& f, FileMatchStack& m)
{
	if (str_pat_mat(to_lower_copy(f.name), name))
	{
		m.push_back(TriBool::True);
		f.cpponly = true;
	} else {
		m.push_back(TriBool::False);
	}
}

void CppOnlyOperator::MatchLines(File& f, LineMatchStack& m)
{
	if (str_pat_mat(to_lower_copy(f.name), name))
	{
		m.emplace_back(true, Lines{});
		f.cpponly = true;
	} else {
		m.emplace_back(false, Lines{});
	}
}
