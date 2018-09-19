
#include "common.h"

std::string unparan(std::string str)
{
	auto sz = str.size();
	assert( sz > 3 );
	assert( str[1] == '(' );
	assert( str.back() == ')' );
	return str.substr(2, sz-3);
}

int getparam(std::string str, int def)
{
	str = str.substr(1);
	if (str.empty()) return def;
	return std::atoi(str.c_str());
}


std::vector<std::string> readfile(std::istream& in)
{
	std::vector<std::string> lines;
	std::string line;
	while (std::getline(in, line))
	{
		lines.push_back(line);
	}
	return lines;
}

#include "operators.h"

#define REG( xx ) xx ## Operator{}.Register()

void register_all()
{
	REG(And);
	REG(Dir);
	REG(File);
	REG(Line);
	REG(Not);
	REG(Or);
}

#undef REG

