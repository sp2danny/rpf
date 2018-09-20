
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void FileOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<FileOperator>(FileOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* FileOperator::clone()
{
	return new FileOperator(*this);
};

char FileOperator::MyChar()
{
	return 'f';
}

void FileOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	name = to_lower_copy(unparan(str));
}

void FileOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void FileOperator::MatchFile ( File& f, FileMatchStack& m )
{
	if (str_pat_mat(to_lower_copy(f.name), name))
		m.push_back(tb_true);
	else
		m.push_back(tb_false);
}

void FileOperator::MatchLines ( File& f, LineMatchStack& m )
{
	if (str_pat_mat(to_lower_copy(f.name), name))
		m.emplace_back(true, Lines{});
	else
		m.emplace_back(false, Lines{});
}

// ----------------------------------------------------------------------------

void CppOnlyOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<CppOnlyOperator>(CppOnlyOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* CppOnlyOperator::clone()
{
	return new CppOnlyOperator(*this);
};

char CppOnlyOperator::MyChar()
{
	return '+';
}

void CppOnlyOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	name = to_lower_copy(unparan(str));
}

void CppOnlyOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void CppOnlyOperator::MatchFile ( File& f, FileMatchStack& m )
{
	if (str_pat_mat(to_lower_copy(f.name), name))
	{
		m.push_back(tb_true);
		f.cpponly = true;
	} else {
		m.push_back(tb_false);
	}
}

void CppOnlyOperator::MatchLines ( File& f, LineMatchStack& m )
{
	if (str_pat_mat(to_lower_copy(f.name), name))
	{
		m.emplace_back(true, Lines{});
		f.cpponly = true;
	} else {
		m.emplace_back(false, Lines{});
	}
}
