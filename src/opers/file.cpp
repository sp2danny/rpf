
#include "../operators.h"

void FileOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<FileOperator>(FileOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

char FileOperator::MyChar()
{
	return 'f';
}

void FileOperator::Create ( std::string str )
{
}

void FileOperator::MatchDir ( File&, FileMatchStack )
{
}

void FileOperator::MatchFile ( File&, FileMatchStack )
{
}

void FileOperator::MatchLines ( File&, LineMatchStack )
{
}


