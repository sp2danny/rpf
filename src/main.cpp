
#include "common.h"
#include "platform.h"
#include "OFM.h"
#include "purgecomment.h"
#include "stringtools.h"
#include "iniparser.h"

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
	bool debugConsidered = false;
	bool debugSearched = false;
	bool debugGeneral = false;
	bool wantClear = false;
	bool impliedAplus = false;
	bool colorLikeLinux = false;
	bool quoteFileNames = false;
	int tab = 8, trunc = 999;
	std::vector<std::string> debugConsideredList;
	std::vector<std::string> debugSearchedList;
}

OperatorStack opStack;

static bool doAllFile(File& f)
{
	FileMatchStack m;
	for (auto&& op : opStack)
	{
		op->MatchFile(f, m);
	}
	if (m.size() != 1)
		throw "operator / operand count error";
	auto res = m.front();
	return res != TriBool::False;
}

std::vector<std::string>& File::lines()
{
	if (!mLoaded)
	{
		std::ifstream ifs{path + "/" + name};
		mLines = readFile(ifs);
		mLoaded = true;
		runstate::sf += 1;
		runstate::sl += mLines.size();
	}
	if (cpponly)
	{
		if (mStripped.empty())
		{
			mStripped = mLines;
			purgeComment(path + "/" + name, mStripped);
		}
		return mStripped;
	}
	return mLines;
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
	#define bold  std::flush; platform::MakeHighlight(); out
	#define reset std::flush; platform::MakeNormal(); out

	out << "Reverse Polish Find : Summary Help\n"

	    << "\n    1a. File Operands (operands produce results)\n"
	    << "\tf(expr)" "\t" << bold << "f" << reset << "ilename matches expr using *? matching, case insensitive\n"
	    << "\t+(expr)" "\t" "like file match above, but c" << bold << "++" << reset << " only, removes line/block\n\t\tcomments and string/character literals from search text\n"
	    << "\td(expr)" "\t" << bold << "d" << reset << "irectory matches expr using *? matching, case insensitive\n"
	    << "\tm(expr)" "\t" "file " << bold << "m" << reset << "odified at or after 'expr', formated as 'YYYY-MM-DD'\n"
	    << "\tx      " "\t" "file is a te" << bold << "x" << reset << "t file. slow, uses mime-type\n"

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
	    << "\t%X"      "\t" "loads saved pattern 'X' from .rpf\n"

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

static void outErr(std::string err)
{
	std::cerr << "error: " << err << std::endl;
}

LineMatch do_all_prio(File&);

void dumpConfig(bool havepath, const std::string& path)
{
	std::cout << " --- Config Dump --- \n";
	if (havepath)
		std::cout << "\thave path : " << path << std::endl;
	else
		std::cout << "\tno path" << std::endl;

	using namespace runstate;
	std::cout << std::boolalpha;
	std::cout << "\tcolorize : "  << (colorize?(colorLikeLinux?"linux":"on"):"off") << std::endl;
	std::cout << "\tstatistic : " << statistic << std::endl;
	std::cout << "\tsparse : "    << sparse    << std::endl;
	std::cout << "\twarnings : "  << warnings  << std::endl;
	std::cout << "\ttabs : "      << tab       << std::endl;
	std::cout << "\ttrunc : "     << runstate::trunc << std::endl;

	std::cout << "\toperators : ";
	for (auto&& op : opStack)
	{
		std::cout << op << " ";
	}
	std::cout << std::endl;

	std::cout << " --- Config Dump --- \n";
}

void doall(std::string path)
{
	using namespace platform;

	RDE rde(path);

	if (runstate::wantClear)
		clearScreen();

	while (auto de = rde.getNext())
	{
		File ff;
		ff.path = de->dirName;
		ff.name = de->fileName;
		ff.cpponly = false;

		runstate::cf += 1;
		if (runstate::debugConsidered)
			runstate::debugConsideredList.push_back(ff.path + "/"s + ff.name);
		if (!doAllFile(ff))
		{
			continue;
		}
		if (runstate::debugSearched)
			runstate::debugSearchedList.push_back(ff.path + "/"s + ff.name);
		auto mm = do_all_prio(ff); // doAllLine(ff);
		if (mm.match())
		{
			OutputFormatter of(2, runstate::tab, runstate::trunc);
			runstate::mf += 1;
			runstate::ml += mm.lines().size();
			if (!runstate::sparse) std::cout << std::endl;
			std::string fn = ff.path + "/" + ff.name;
			if (runstate::quoteFileNames)
				fn = "'"s + fn + "'"s; 
			of.LineOut(fn);
			ff.cpponly = false;
			UL lastline;
			bool first = true;
			for (auto&& ln : mm.lines())
			{
				if (!runstate::sparse)
				{
					if (first)
					{
						of.LineOut("");
						first = false;
					}
					else if (ln.first != (lastline+1))
					{
						of.LineOut("");
					}
					lastline = ln.first;
				}
				//std::cout << runstate::tab << (ln.first+1) << runstate::tab;
				const auto& l = ff.lines()[ln.first];
				UL i, n = l.size();
				std::string ss;
				{
					bool ingr = false;
					for (i=0; i<n; ++i)
					{
						bool hc = mm.haveChar(ln.first, i);
						if (hc)
						{
							if (!ingr)
							{
								ingr = true;
								ss += "%g%";
							}
						} else {
							if (ingr)
							{
								ingr = false;
								ss += "%n%";
							}
						}
						ss += l[i];
					}
					if (ingr)
						ss += "%n%";
				}
				of.ColumnsOut( std::to_string(ln.first+1), ss );
			}
			if (!runstate::sparse) of.LineOut("");
			of.OutAndClear(runstate::colorize, std::cout);
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
	std::sort(opsp.begin(), opsp.end());
	auto iter = std::unique(opsp.begin(), opsp.end());
	opsp.erase(iter, opsp.end());
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
		if (lm.front().tri() == TriBool::False)
			return {false, {}};
	}
	if (lm.front().tri() == TriBool::Maybe)
		throw "internal engine error";
	return std::move(lm.front());
}

void add_op(OperatorStack& ops, std::string arg, const IniFile& ini)
{
	if (arg.empty()) return;
	if (arg.front() == '%')
	{
		auto name = arg.substr(1);
		if (!ini.HasKey("save", name))
		{
			throw "saved term not found";
		}
		auto val = ini.Lookup("save", name);
		std::istringstream iss;
		iss.str(val);
		std::string token;
		while (iss >> token)
		{
			ops.push_back(Operator::DispatchCreate(token));
		}
	} else {
		ops.push_back(Operator::DispatchCreate(arg));
	}
}

int main(int argc, char** argv)
{
	registerAll();
	
	runstate::colorize = platform::stdoutIsatty();

	IniFile ini;
	recursiveLoad(ini,".rpf");

	ini.AssignIfSet("general", "stats",            runstate::statistic);
	ini.AssignIfSet("general", "trunc",            runstate::trunc);
	ini.AssignIfSet("general", "tabs",             runstate::tab);
	ini.AssignIfSet("general", "sparse",           runstate::sparse);
	ini.AssignIfSet("general", "color",            runstate::colorize);
	ini.AssignIfSet("general", "warnings",         runstate::warnings);
	ini.AssignIfSet("general", "implied_aplus",    runstate::impliedAplus);
	ini.AssignIfSet("general", "color_like_linux", runstate::colorLikeLinux);

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
		std::cout << "Reverse Polish Find v1.0.13" << std::endl;
	}
	else try
	{
		std::string path;
		bool have_path = false;
		bool final_config_dump = false;
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
				else if (arg == "color-like-linux-on")
					runstate::colorLikeLinux = true;
				else if (arg == "color-like-linux-off")
					runstate::colorLikeLinux = false;
				else if (arg == "quote-file-names-on")
					runstate::quoteFileNames = true;
				else if (arg == "quote-file-names-off")
					runstate::quoteFileNames = false;
				else if (arg == "debug-considered")
					runstate::debugConsidered = true;
				else if (arg == "debug-searched")
					runstate::debugSearched = true;
				else if (arg == "debug")
					runstate::debugGeneral = runstate::debugSearched = runstate::debugConsidered = true;
				else if ((arg == "reset") || (arg == "clear"))
					runstate::wantClear = true;
				else if (arg.substr(0, 5) == "tabs-")
					runstate::tab = std::stoi(arg.substr(5));
				else if (arg.substr(0, 6) == "trunc-")
					runstate::trunc = std::stoi(arg.substr(6));
				else if (arg == "final-config-dump")
					final_config_dump = true;
				else
					throw "Unknown argument "s + arg;
			}
			else
			{
				if (have_path)
				{
					add_op(opStack, arg, ini);
				} else {
					have_path = true;
					path = arg;
				}
			}
		}
		if (runstate::debugGeneral)
			debug_listing();
		if (runstate::impliedAplus)
			add_op(opStack, "a+", ini);
		if (final_config_dump)
			dumpConfig(have_path, path);
		if (!have_path)
			throw "nothing to do";
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

		if (runstate::debugConsidered)
		{
			std::cout << "*** DEBUG *** CONSIDERED ***" << std::endl;
			for (auto&& str : runstate::debugConsideredList)
				std::cout << str << std::endl;
		}

		if (runstate::debugSearched)
		{
			std::cout << "*** DEBUG *** SEARCHED ***" << std::endl;
			for (auto&& str : runstate::debugSearchedList)
				std::cout << str << std::endl;
		}

	}
	catch (const std::string& err)
	{
		outErr(err);
		return -1;
	}
	catch (const char* err)
	{
		outErr(err);
		return -1;
	}
	catch (const std::exception& err)
	{
		outErr(err.what());
		return -1;
	}
	catch (...)
	{
		outErr("unknown exception");
		return -1;
	}

	return 0;
}
