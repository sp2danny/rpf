
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

// ***********
// *** Dup ***
// ***********

void DupOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<DupOperator>(DupOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* DupOperator::clone()
{
	return new DupOperator(*this);
};

char DupOperator::MyChar()
{
	return '2';
}

void DupOperator::Create ( [[maybe_unused]] std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() != 1)
		throw "operator syntax error";
}

void DupOperator::MatchDir ( File&, FileMatchStack& m )
{
	if (m.size() < 1)
	{
		throw "operator dup: not enough operands";
	}
	TriBool res = m.back();
	m.push_back(res);
}

void DupOperator::MatchFile ( File& f, FileMatchStack& m )
{
	MatchDir(f, m);
}

void DupOperator::MatchLines ( File& , LineMatchStack& m )
{
	if (m.size() < 1)
	{
		throw "operator dup: not enough operands";
	}
	auto res = m.back();
	m.push_back(res);
}

// ----------------------------------------------------------------------------

// ************
// *** Swap ***
// ************

void SwapOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<SwapOperator>(SwapOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* SwapOperator::clone()
{
	return new SwapOperator(*this);
};

char SwapOperator::MyChar()
{
	return 's';
}

void SwapOperator::Create ( [[maybe_unused]] std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() != 1)
		throw "operator syntax error";
	pos = getparam(str, 2);
}

void SwapOperator::MatchDir ( File&, FileMatchStack& m )
{
	if (m.size() < pos)
	{
		throw "operator swap: not enough operands";
	}
	auto& m1 = *(m.end()-1);
	auto& m2 = *(m.end()-pos);
	using std::swap;
	swap(m1, m2);
}

void SwapOperator::MatchFile ( File& f, FileMatchStack& m )
{
	MatchDir(f, m);
}

void SwapOperator::MatchLines ( File& , LineMatchStack& m )
{
	if (m.size() < pos)
	{
		throw "operator swap: not enough operands";
	}
	auto& m1 = *(m.end()-1);
	auto& m2 = *(m.end()-pos);
	using std::swap;
	swap(m1, m2);
}



