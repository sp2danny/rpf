
#include "common.h"
#include "OFM.h"

#ifdef SUPPORT_NEW_SYNTAX
std::string unparan(std::string str)
{
	auto sz = str.size();
	assert( sz >= 2 );
	if (str[1] == '(')
	{
		assert( sz > 3 );
		assert( str.back() == ')' );
		return str.substr(2, sz-3);
	}
	else if (str[1] == '=')
	{
		return str.substr(2);
	}
	else
		throw "operator syntax error";
}
#else
std::string unparan(std::string str)
{
	auto sz = str.size();
	assert( sz > 3 );
	assert( str[1] == '(' );
	assert( str.back() == ')' );
	return str.substr(2, sz-3);
}
#endif

int getparam(std::string str, int def)
{
	//[[expects: (def>=0)]]
	str = str.substr(1);
	if (str.empty()) return def;
	return std::stoi(str);
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
	REG(CppOnly);
	REG(Line);
	REG(LineCI);
	REG(Not);
	REG(Or);
	REG(Near);
	REG(BM);
	REG(BM_CI);
	REG(Regex);
	REG(Sub);
	REG(True);
	REG(Inv);
	REG(Mod);
	REG(Ident);
	REG(IdentCI);
	REG(Range);
	REG(Dup);
	REG(Swap);
	REG(Text);
}

#undef REG

TriBool And(TriBool lhs, TriBool rhs)
{
	switch (lhs)
	{
	case tb_false:
		return tb_false;
	case tb_maybe:
		return (rhs==tb_false) ? tb_false : tb_maybe;
	case tb_true:
		return rhs;
	default:
		throw "internal engine error";
	}
}

TriBool Or(TriBool lhs, TriBool rhs)
{
	switch (lhs)
	{
	case tb_true:
		return tb_true;
	case tb_maybe:
		return (rhs==tb_true) ? tb_true : tb_maybe;
	case tb_false:
		return rhs;
	default:
		throw "internal engine error";
	}
}

TriBool Not(TriBool arg)
{
	switch (arg)
	{
	case tb_true:
		return tb_false;
	case tb_maybe:
		return tb_maybe;
	case tb_false:
		return tb_true;
	default:
		throw "internal engine error";
	}
}

TriBool FromBool(bool b)
{
	return b ? tb_true : tb_false;
}


std::string to_string(TriBool tb)
{
	switch (tb)
	{
		case tb_false: return "false"s;
		case tb_true:  return "true"s;
		case tb_maybe: return "maybe"s;
		default:       return "<error>"s;
	}
}

void print_stack(const FileMatchStack& s)
{
	std::cout << "stack contains:" << std::endl;
	for (auto&& x : s)
	{
		std::cout << to_string(x) << std::endl;
	}
}
void print_stack(const LineMatchStack& s)
{
	std::cout << "stack contains:" << std::endl;
	for (auto&& x : s)
	{
		std::cout << to_string(x.tri()) << std::endl;
	}
}

#include "platform.h"

namespace like_linux
{

	const std::string red   = "\033[0;91m";
	const std::string green = "\033[0;32m";
	const std::string reset = "\033[0m";
	const std::string bold  = "\033[1;37m";
	const std::string clear = "\033[2J\033[1;1H";

	void clear_screen()   { std::cout << clear; }
	void MakeHighlight()  { std::cout << bold;  }
	void MakeNormal()     { std::cout << reset; }
	void MakeRed()        { std::cout << red;   }
	void MakeGreen()      { std::cout << green; }

}

