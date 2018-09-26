
#include "../operators.h"
#include "../common.h"

using boost::algorithm::to_lower_copy;
using boost::algorithm::to_lower;

char LineOperator::MyChar()
{
	return 'l';
}

void LineOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	expr = unparan(str);
}

/*void LineOperator::MatchDir ( File&, FileMatchStack& m )
{
	m.push_back(tb_maybe);
}*/

void LineOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void LineOperator::MatchLines ( File& f, LineMatchStack& m )
{
	LinesCache(f);
	ExeCached(m);
}

void LineOperator::LinesCache(File& f)
{
	if (have_cache) return;
	UL ln = 0;
	for (auto&& l : f.lines())
	{
		if (str_pat_mat(l, expr))
		{
			auto vzzp = str_pat_mat_special(l, expr);
			if (vzzp.empty())
				lm_cache.add_simple_match(ln);
			else
				for(auto&& zzp : vzzp)
					lm_cache.add_full_match(ln, zzp.first, zzp.second);
		}
		++ln;
	}
	have_cache = true;
}

bool LineOperator::IsCached() { return have_cache; }

int LineOperator::MyPrio() { return 5; }

void LineOperator::ExeCached(LineMatchStack& lms)
{
	if (have_cache)
		lms.push_back(lm_cache);
	else
		lms.push_back({tb_maybe});
}

// ----------------------------------------------------------------------------

char LineCIOperator::MyChar()
{
	return 'L';
}

void LineCIOperator::Create ( std::string str )
{
	assert(!str.empty());
	assert(str[0] == MyChar());
	expr = to_lower_copy(unparan(str));
}

void LineCIOperator::MatchFile ( File& , FileMatchStack& m )
{
	m.push_back(tb_maybe);
}

void LineCIOperator::MatchLines ( File& f, LineMatchStack& m )
{
	LineMatch res;
	UL ln = 0;
	for (auto&& l : f.lines())
	{
		auto lci = to_lower_copy(l);
		if (str_pat_mat(lci, expr))
		{
			auto vzzp = str_pat_mat_special(lci, expr);
			if (vzzp.empty())
				res.add_simple_match(ln);
			else
				for(auto&& zzp : vzzp)
					res.add_full_match(ln, zzp.first, zzp.second);
		}
		++ln;
	}
	m.push_back(std::move(res));
}
