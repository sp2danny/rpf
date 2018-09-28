
#include "common.h"

#include "purgecomment.h"

#include "container_operations.hpp"

std::map<char, OperatorMaker> Operator::createMap;

void Operator::Register(char c, OperatorMaker opm)
{
	auto n = createMap.count(c);
	if (n)
		throw "internal error: duplicate operator symbol";
	createMap[c] = opm;
}

clone_ptr<Operator> Operator::DispatchCreate(std::string str)
{
	assert(!str.empty());
	char c = str[0];
	auto iter = createMap.find(c);
	if (iter == createMap.end())
		throw "unrecognized operator "s + c;
	return iter->second(str);
}

namespace runstate
{
	unsigned long long ml = 0, mf = 0, cf=0, sf=0, sl=0;
	bool colorize, statistic = true, sparse = true, warnings = true;
	bool debug_considered = false;
	bool debug_searched = false;
	bool debug_general = false;
	bool want_clear = false;
	std::string tab = "\t";
	std::vector<std::string> debug_considered_list;
	std::vector<std::string> debug_searched_list;
}

OperatorStack opStack;

bool do_all_file(File& f)
{
	FileMatchStack m;
	for (auto&& op : opStack)
	{
		op->MatchFile(f, m);
	}
	if (m.size() != 1)
		throw "operator / operand count error";
	auto res = m.front();
	return res != tb_false;
}

LineMatch do_all_line(File& f)
{
	LineMatchStack lm;
	UL i, j, n = opStack.size();
	for (i=0; i<n; ++i)
	{
		auto&& opi = opStack[i];
		opi->MatchLines(f, lm);
		FileMatchStack fm;
		for (auto&& lmi : lm)
			fm.push_back(FromBool(lmi.match()));
		for(j=i+1; j<n; ++j)
		{
			auto&& opj = opStack[j];
			opj->MatchFile(f, fm);
		}
		if (fm.size() != 1)
			throw "operator / operand count error";
		if (fm.front() == tb_false)
			return {false, {}};
	}
	if (lm.size() != 1)
		throw "operator / operand count error";
	return lm.front();
}

std::vector<std::string>& File::lines()
{
	if (!m_loaded)
	{
		std::ifstream ifs{path + "/" + name};
		m_lines = readfile(ifs);
		m_loaded = true;
		runstate::sf += 1;
		runstate::sl += m_lines.size();
	}
	if (cpponly)
	{
		if (m_stripped.empty())
		{
			m_stripped = m_lines;
			purge_comment(path + "/" + name, m_stripped);
		}
		return m_stripped;
	}
	return m_lines;
}

void Usage(std::ostream& out)
{
	out << "rpf: Usage:\n"
	    << "\trpf                           - usage\n"
	    << "\trpf --help                    - brief help\n"
	    << "\trpf --version                 - print version\n"
	    << "\trpf <dir> [options] |terms|   - search dir using terms\n"
	;
}

void Help(std::ostream& out) // print help to stdout
{
	#define bold  std::flush; MakeHighlight(); out
	#define reset std::flush; MakeNormal(); out

	out << "Reverse Polish Find : Summary Help\n"

	    << "\n    1a. File Operands (operands produce results)\n"
	    << "\tf(expr)" "\t" << bold << "f" << reset << "ilename matches expr using *? matching, case insensitive\n"
	    << "\t+(expr)" "\t" "like file match above, but c" << bold << "++" << reset << " only, removes line/block\n\t\tcomments and string/character literals from search text\n"
	    << "\td(expr)" "\t" << bold << "d" << reset << "irectory matches expr using *? matching, case insensitive\n"
	    << "\tm(expr)" "\t" "file " << bold << "m" << reset << "odified at or after 'expr', formated as 'YYYY-MM-DD'\n"

	    << "\n    1b. Text Operands (produce results, and populates match-lines)\n"
	    << "\tl(expr)" "\t" "whole " << bold << "l" << reset << "ine matches expr using *? matching, case sensitive\n"
	    << "\tL(expr)" "\t" "whole " << bold << "l" << reset << "ine matches expr using *? matching, case insensitive\n"
	    << "\ti(expr)" "\t" "tokenizes line, expr matches one " << bold << "i" << reset << "dentifier exactly\n"
	    << "\tI(expr)" "\t" "tokenizes line, expr matches one " << bold << "i" << reset << "dentifier case insensitive\n"
	    << "\tr(expr)" "\t" "part of line matches expr using " << bold << "r" << reset << "egex matching\n"
	    << "\tb(expr)" "\t" "part of line matches expr using " << bold << "B" << reset << "oyerMoore matching, no *?\n\t\tmatching\n"
	    << "\tB(expr)" "\t" "part of line matches expr using " << bold << "B" << reset << "oyerMoore matching, no *?\n\t\tmatching, case insesitive\n"

	    << "\n    2. Operators (operators combine results)\n"
	    << "\ta"       "\t" << bold << "a" << reset << "nd's 2 results, merges match-lines\n"
	    << "\to"       "\t" << bold << "o" << reset << "r's 2 results, merges match-lines\n"
	    << "\tn"       "\t" << bold << "n" << reset << "ear, ie and's 2 results, remove match-lines that are more\n\t\tapart than 5 lines\n"
	    << "\tnN"      "\t" << bold << "n" << reset << "ear, ie and's 2 results, remove match-lines that are more\n\t\tapart than N lines\n"
	    << "\t!"       "\t" "negates's 1 results, purge match-lines\n"
	    << "\t-"       "\t" "subtracts later match-lines from former\n"
	    << "\t~"       "\t" "inverts match-lines from result\n"

	    << "\n    3. Misc\n"
	    << "\tt"       "\t" "always " << bold << "t" << reset << "rue, does not populate match-lines\n"

	    << "\n    4. Options (specify with --option-on or --option-off)\n"
	    << "\tsparse"  "\tproduce output without separating blank lines.\n\t\tdefault on\n"
	    << "\tcolor"   "\tproduce output with colorcoded matches.\n\t\tdefault on if output is a tty\n"
	    << "\tstats"   "\tsummary statistic printed.\n\t\tdefault on\n"

	    << "\n    Notes\n"
	    << "\tOnly l, r, i and b (and uppercase variants) loads the file-content\n"
	    << "\tMany operators and operands need to be escaped or quoted\n"
	;

	#undef bold
	#undef reset
}

void out_err(std::string err)
{
	std::cerr << "error: " << err << std::endl;
}

LineMatch do_all_prio(File&);

void doall(std::string path)
{
	RDE rde(path);
	
	if (runstate::want_clear)
		clear_screen();

	while (auto de = rde.getNext())
	{
		File ff;
		ff.path = de->dir_name;
		ff.name = de->file_name;
		ff.cpponly = false;

		runstate::cf += 1;
		if (runstate::debug_considered)
			runstate::debug_considered_list.push_back(ff.path + "/"s + ff.name);
		if (!do_all_file(ff))
		{
			continue;
		}
		if (runstate::debug_searched)
			runstate::debug_searched_list.push_back(ff.path + "/"s + ff.name);
		auto mm = do_all_prio(ff); // do_all_line(ff);
		if (mm.match())
		{
			runstate::mf += 1;
			runstate::ml += mm.lines().size();
			if (!runstate::sparse) std::cout << std::endl;
			std::cout << ff.path + "/" + ff.name << std::endl;
			ff.cpponly = false;
			UL lastline;
			bool first = true;
			for (auto&& ln : mm.lines())
			{
				if (!runstate::sparse)
				{
					if (first)
					{
						std::cout << std::endl;
						first = false;
					}
					else if (ln.first != (lastline+1))
					{
						std::cout << std::endl;
					}
					lastline = ln.first;
				}
				std::cout << runstate::tab << (ln.first+1) << runstate::tab;
				const auto& l = ff.lines()[ln.first];
				UL i, n = l.size();
				if (runstate::colorize)
				{
					bool ingr = false;
					for (i=0; i<n; ++i)
					{
						bool hc = mm.have_char(ln.first, i);
						if (hc)
						{
							if (!ingr)
							{
								ingr = true;
								MakeGreen();
							}
						} else {
							if (ingr)
							{
								ingr = false;
								MakeNormal();
							}
						}
						if (l[i] == '\t')
							std::cout << runstate::tab;
						else
							std::cout << l[i];
					}
					if (ingr)
						MakeNormal();
				} else {
					for (i=0; i<n; ++i)
						if (l[i] == '\t')
							std::cout << runstate::tab;
						else
							std::cout << l[i];
				}
				std::cout << std::endl;
			}
			if (!runstate::sparse) std::cout << std::endl;
		}
	}
}

void debug_listing()
{
	std::list<int> opsp;
	for (auto&& op : opStack)
	{
		opsp.push_back(op->MyPrio());
	}
	std::cout << "ops : ";
	for (int i : opsp)
		std::cout << i << " ";
	std::cout << std::endl << "ops : ";
	opsp.sort(); opsp.unique();
	for (int i : opsp)
		std::cout << i << " ";
	std::cout << std::endl;
}

LineMatch do_all_prio(File& f)
{
	std::vector<int> opsp;
	for (auto&& op : opStack)
	{
		op->UnCache();
		opsp.push_back(op->MyPrio());
	}
	sort(opsp); unique(opsp);
	if (opsp.empty())
		throw "nothing to do";
	LineMatchStack lm;
	for (auto p : opsp)
	{
		for (auto&& op : opStack)
		{
			if (op->MyPrio() == p)
				op->LinesCache(f);
		}
		lm.clear();
		for (auto&& op : opStack)
		{
			op->ExeCached(lm);
		}
		if (lm.size() != 1)
			throw "operator / operand count error";
		if (lm.front().tri() == tb_false)
			return {false, {}};
	}
	if (lm.front().tri() == tb_maybe)
		throw "internal engine error";
	return std::move(lm.front());
}

void add_op(OperatorStack& ops, std::string arg)
{
	ops.push_back( Operator::DispatchCreate(arg) );
}

std::string maketabs(std::string s)
{
	int i, n = std::stoi(s);
	std::string ret;
	for(i=0;i<n;++i) ret += " ";
	return ret;
}

int main(int argc, char** argv)
{
	register_all();

	runstate::colorize = stdout_isatty();

	if (argc<=1)
	{
		Usage(std::cout);
	}
	else if ((argc==2) && (argv[1]=="--help"s))
	{
		Help(std::cout);
	}
	else if ((argc==2) && (argv[1]=="--version"s))
	{
		std::cout << "Reverse Polish Find v1.04" << std::endl;
	}
	else try
	{
		std::string path;
		bool have_path = false;
		for (int i=1; i<argc; ++i)
		{
			std::string arg = argv[i];
			if (arg.substr(0,2) == "--")
			{
				arg = arg.substr(2);
				/**/ if (arg == "color-on")
					runstate::colorize = true;
				else if (arg == "color-off")
					runstate::colorize = false;
				else if (arg == "stats-on")
					runstate::statistic = true;
				else if (arg == "stats-off")
					runstate::statistic = false;
				else if (arg == "sparse-on")
					runstate::sparse = true;
				else if (arg == "sparse-off")
					runstate::sparse = false;
				else if (arg == "warnings-on")
					runstate::warnings = true;
				else if (arg == "warnings-off")
					runstate::warnings = false;
				else if (arg == "debug-considered")
					runstate::debug_considered = true;
				else if (arg == "debug-searched")
					runstate::debug_searched = true;
				else if (arg == "debug")
					runstate::debug_general = runstate::debug_searched = runstate::debug_considered = true;
				else if ((arg == "reset") || (arg == "clear"))
					runstate::want_clear = true;
				else if (arg.substr(0,5) == "tabs-")
					runstate::tab = maketabs(arg.substr(5));
				else
					throw "Unknown argument "s + arg;
			}
			else
			{
				if (have_path)
				{
					add_op(opStack, arg);
				} else {
					have_path = true;
					path = arg;
				}
			}
		}
		if (!have_path)
			throw "nothing to do";
		if (runstate::debug_general)
			debug_listing();
		doall(path);
		if (runstate::statistic)
		{
			if (runstate::sparse) std::cout << std::endl;
			std::cout << "Files Matched    : " << runstate::mf << std::endl;
			std::cout << "Lines Matched    : " << runstate::ml << std::endl;
			std::cout << "Considered Files : " << runstate::cf << std::endl;
			std::cout << "Searched Files   : " << runstate::sf << std::endl;
			std::cout << "Searched Lines   : " << runstate::sl << std::endl;
		}

		if (runstate::debug_considered)
		{
			std::cout << "*** DEBUG *** CONSIDERED ***" << std::endl;
			for (auto&& str : runstate::debug_considered_list)
				std::cout << str << std::endl;
		}

		if (runstate::debug_searched)
		{
			std::cout << "*** DEBUG *** SEARCHED ***" << std::endl;
			for (auto&& str : runstate::debug_searched_list)
				std::cout << str << std::endl;
		}

	}
	catch (const std::string& err)
	{
		out_err(err);
		return -1;
	}
	catch (const char* err)
	{
		out_err(err);
		return -1;
	}
	catch (const std::exception& err)
	{
		out_err(err.what());
		return -1;
	}
	catch (...)
	{
		out_err("unknown exception");
		return -1;
	}
}



