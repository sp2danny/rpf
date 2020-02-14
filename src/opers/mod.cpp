
#include "../operators.h"
#include "../common.h"
#include "../platform.h"

#include <ctime>

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

void ModOperator::Print(std::ostream& out) const
{
	using namespace std;
	char buff[16];
	struct tm _tm = *localtime(&md);
	strftime(buff, 16, "%Y-%m-%d", &_tm);
	out << "m='" << buff << "'";
}

char ModOperator::MyChar()
{
	return 'm';
}

void ModOperator::Create(std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	md = platform::parseDateFromString(unParan(str));
}

static bool do_mod(File& f, std::time_t md)
{
	auto filename = f.path + "/" + f.name;
	auto filetime = platform::getModificationTimeFromFile(filename);

	return (filetime >= md);
}

void ModOperator::MatchFile(File& f, FileMatchStack& m)
{
	TriBool res = FromBool(do_mod(f, md));
	m.push_back(res);
}

void ModOperator::MatchLines(File& f, LineMatchStack& m)
{
	m.emplace_back(do_mod(f, md), Lines{});
}
