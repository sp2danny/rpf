
#include "../operators.h"
#include "../common.h"
#include "../stringtools.h"

#include <magic.h>

char TextOperator::MyChar()
{
	return 'x';
}

void TextOperator::Create ( [[maybe_unused]] std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
}

static bool test_file_text(File& f)
{
	std::string fn = f.path + "/" + f.name;
	
	const char *mime;
	static magic_t magic;
	static bool first = true;

	if (first)
	{
		magic = magic_open(MAGIC_MIME_TYPE); 
		magic_load(magic, NULL);
		magic_compile(magic, NULL);
		first = false;
	}
	mime = magic_file(magic, fn.c_str());
	//magic_close(magic);

	std::string mimestr = mime;
	//std::cerr << "debug log, mime = " << mimestr << std::endl;
	return mimestr.find("text") != std::string::npos;
}

void TextOperator::MatchFile ( File& f, FileMatchStack& m )
{
	UnCache();
	LinesCache(f);
	if (res)
		m.push_back(tb_true);
	else
		m.push_back(tb_false);
}

void TextOperator::MatchLines ( File& f, LineMatchStack& m )
{
	LinesCache(f);
	if (res)
		m.emplace_back(true, Lines{});
	else
		m.emplace_back(false, Lines{});
}

void TextOperator::LinesCache(File& f)
{
	if (have_cache) return;
	res = test_file_text(f);
	have_cache = true;
}

bool TextOperator::IsCached() { return have_cache; }

int TextOperator::MyPrio() { return 2; }

void TextOperator::ExeCached(LineMatchStack& lms)
{
	if (have_cache)
		lms.push_back({res, {}});
	else
		lms.push_back({tb_maybe});
}

void TextOperator::UnCache()
{
	have_cache = false;
	lm_cache = LineMatch{false,{}};
}

