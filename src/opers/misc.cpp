
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

// ***********
// *** Dup ***
// ***********

void DupOperator::Print(std::ostream& out) const
{
	out << "2";
}

char DupOperator::MyChar()
{
	return '2';
}

void DupOperator::Create([[maybe_unused]] std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() != 1)
		throw "operator syntax error";
}

void DupOperator::MatchFile([[maybe_unused]] File& f, FileMatchStack& m)
{
	if (m.size() < 1)
	{
		throw "operator dup: not enough operands";
	}
	TriBool res = m.back();
	m.push_back(res);
}

void DupOperator::MatchLines(File&, LineMatchStack& m)
{
	if (m.size() < 1)
	{
		throw "operator dup: not enough operands";
	}
	auto res = m.back();
	m.push_back(std::move(res));
}

// ----------------------------------------------------------------------------

// ************
// *** Swap ***
// ************

void SwapOperator::Print(std::ostream& out) const
{
	out << "s";
}

char SwapOperator::MyChar()
{
	return 's';
}

void SwapOperator::Create([[maybe_unused]] std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	if (str.size() != 1)
		throw "operator syntax error";
	pos = getParam(str, 2);
}

void SwapOperator::MatchFile([[maybe_unused]] File& f, FileMatchStack& m)
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

void SwapOperator::MatchLines(File&, LineMatchStack& m)
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



