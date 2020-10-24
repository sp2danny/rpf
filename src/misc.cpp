
#include "common.h"
#include "OFM.h"

std::string unParan(std::string str)
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

int getParam(std::string str, int def)
{
	//[[expects: (def>=0)]]
	str = str.substr(1);
	if (str.empty()) return def;
	return std::stoi(str);
}

std::vector<std::string> readFile(std::istream& in)
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

void registerAll()
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

auto And(TriBool lhs, TriBool rhs) -> TriBool
{
	switch (lhs)
	{
	case TriBool::False:
		return TriBool::False;
	case TriBool::Maybe:
		return (rhs == TriBool::False) ? TriBool::False : TriBool::Maybe;
	case TriBool::True:
		return rhs;
	default:
		throw "internal engine error";
	}
}

auto Or(TriBool lhs, TriBool rhs) -> TriBool
{
	switch (lhs)
	{
	case TriBool::True:
		return TriBool::True;
	case TriBool::Maybe:
		return (rhs == TriBool::True) ? TriBool::True : TriBool::Maybe;
	case TriBool::False:
		return rhs;
	default:
		throw "internal engine error";
	}
}

auto Not(TriBool arg) -> TriBool
{
	switch (arg)
	{
	case TriBool::True:
		return TriBool::False;
	case TriBool::Maybe:
		return TriBool::Maybe;
	case TriBool::False:
		return TriBool::True;
	default:
		throw "internal engine error";
	}
}

auto FromBool(bool b) -> TriBool
{
	return b ? TriBool::True : TriBool::False;
}

std::string to_string(TriBool tb)
{
	switch (tb)
	{
	case TriBool::False: return "false"s;
	case TriBool::True:  return "true"s;
	case TriBool::Maybe: return "maybe"s;
	default:             return "<error>"s;
	}
}

void printStack(const FileMatchStack& s)
{
	std::cout << "stack contains:" << std::endl;
	for (auto&& x : s)
	{
		std::cout << to_string(x) << std::endl;
	}
}
void printStack(const LineMatchStack& s)
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

	void clearScreen()    { std::cout << clear; }
	void MakeHighlight()  { std::cout << bold;  }
	void MakeNormal()     { std::cout << reset; }
	void MakeRed()        { std::cout << red;   }
	void MakeGreen()      { std::cout << green; }

}


