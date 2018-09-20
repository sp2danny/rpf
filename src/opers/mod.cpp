
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void ModOperator::Register()
{
	auto maker = [](std::string s) -> clone_ptr<Operator>
	{
		auto o = clone_ptr<ModOperator>(ModOperator{s});
		return clone_ptr<Operator>(o);
	};
	Operator::Register( MyChar() , +maker );
}

Operator* ModOperator::clone()
{
	return new ModOperator(*this);
};

char ModOperator::MyChar()
{
	return 'm';
}

void ModOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	md = parse_date_from_string(unparan(str));
}

void ModOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

static bool do_mod(File& f, std::time_t md)
{
	auto filename = f.path + "/" + f.name;
	auto filetime = get_modification_time_from_file(filename);

	return (filetime >= md);
}

void ModOperator::MatchFile ( File& f, FileMatchStack& m )
{
	TriBool res = FromBool(do_mod(f, md));
	m.push_back(res);
}

void ModOperator::MatchLines ( File& f, LineMatchStack& m )
{
	m.emplace_back(do_mod(f, md), Lines{});
}
