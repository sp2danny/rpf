
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void IdentOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<IdentOperator>(IdentOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* IdentOperator::clone()
{
	return new IdentOperator(*this);
};

char IdentOperator::MyChar()
{
	return 'i';
}

void IdentOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	id = unparan(str);
}

void IdentOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void IdentOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void IdentOperator::MatchLines ( File& f, LineMatchStack& m )
{
	LineMatch res;
	UL ln = 0;
	UL sz = id.size();
	for (auto&& l : f.lines())
	{
		auto vsp = tokenify(l);
		for (auto&& t : vsp)
		{
			if (t.first == id)
			{
				res.add_full_match(ln, t.second, t.second+sz);
			}
		}
		++ln;
	}
	m.push_back(std::move(res));
}

// ----------------------------------------------------------------------------

void IdentCIOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<IdentCIOperator>(IdentCIOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* IdentCIOperator::clone()
{
	return new IdentCIOperator(*this);
};

char IdentCIOperator::MyChar()
{
	return 'I';
}

void IdentCIOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	id = to_lower_copy(unparan(str));
}

void IdentCIOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void IdentCIOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void IdentCIOperator::MatchLines ( File& f, LineMatchStack& m )
{
	LineMatch res;
	UL ln = 0;
	UL sz = id.size();
	for (auto&& l : f.lines())
	{
		auto vsp = tokenify(l);
		for (auto&& t : vsp)
		{
			if (to_lower_copy(t.first) == id)
			{
				res.add_full_match(ln, t.second, t.second+sz);
			}
		}
		++ln;
	}
	m.push_back(std::move(res));
}
