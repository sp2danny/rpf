
#include "../operators.h"
#include "../common.h"
#include "../stringtools.h"
#include "../platform.h"

void TextOperator::Print(std::ostream& out) const
{
	out << "x";
}

char TextOperator::MyChar()
{
	return 'x';
}

void TextOperator::Create([[maybe_unused]] std::string str)
{
	assert(!str.empty());
	assert(str[0] == MyChar());
}

static bool testFileText(File& f)
{
	std::string fn = f.path + "/" + f.name;
	std::string mimestr = platform::GetMimeType(fn);
	return mimestr.find("text") != std::string::npos;
}

void TextOperator::MatchFile(File& f, FileMatchStack& m)
{
	UnCache();
	LinesCache(f);
	if (res)
		m.push_back(TriBool::True);
	else
		m.push_back(TriBool::False);
}

void TextOperator::MatchLines(File& f, LineMatchStack& m)
{
	LinesCache(f);
	if (res)
		m.emplace_back(true, Lines{});
	else
		m.emplace_back(false, Lines{});
}

void TextOperator::LinesCache(File& f)
{
	if (mHaveCache) return;
	res = testFileText(f);
	mHaveCache = true;
}

bool TextOperator::IsCached() { return mHaveCache; }

int TextOperator::MyPrio() { return 2; }

void TextOperator::ExeCached(LineMatchStack& lms)
{
	if (mHaveCache)
		lms.push_back({res, {}});
	else
		lms.push_back({TriBool::Maybe});
}

void TextOperator::UnCache()
{
	mHaveCache = false;
	mLmCache = LineMatch{false,{}};
}

