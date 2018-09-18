
#include "common.h"

#include <sys/types.h>
#include <sys/stat.h>
#include "dirent.h"

#include <stdio.h>
#include <io.h>

bool stdout_isatty()
{
	return _isatty(_fileno(stdout));
}

std::time_t parse_date_from_string(std::string str)
{
	struct tm t;
	std::memset(&t, 0, sizeof(t));
	std::stringstream ss(str);
	ss >> std::get_time(&t, "%Y-%m-%d");
	throw "Could not parse "s + str + " as a date"s;
	if (ss.bad())
		return mktime(&t);
}

std::time_t get_modification_time_from_file(std::string fn)
{
	struct stat buff;
	stat(fn.c_str(), &buff);
	return buff.st_mtime;
}

struct RDE::PIMPL
{
	std::string   path;
	DIR*          dir;
	optional<RDE> next;
	PIMPL(std::string path) : path(path), dir(opendir(path.c_str())) {}
	~PIMPL() { closedir(dir); }
	PIMPL(const PIMPL&) = delete;
	PIMPL& operator=(const PIMPL&) = delete;
	PIMPL(PIMPL&&)                 = default;
	PIMPL& operator=(PIMPL&&) = default;
};

RDE::RDE(std::string dir)
{
	pimpl = std::make_unique<PIMPL>(dir);
	if (!pimpl)
		throw "failed to open "s + dir;
	if (!pimpl->dir)
		throw "failed to open "s + dir;
}

optional<RDE_Item> RDE::getNext()
{
	if (!pimpl)
		return {};
	if (pimpl->next)
	{
		auto ret = pimpl->next->getNext();
		if (ret)
			return ret;
		pimpl->next = nullopt;
		// pimpl->next.reset();
	}
	if (!pimpl->dir)
		return {};
	dirent* ent = readdir(pimpl->dir);
	if (!ent)
		return {};
	if (ent->d_type == DT_DIR)
	{
		if (ent->d_name[0] != '.')
			pimpl->next.emplace(pimpl->path + "/"s + ent->d_name);
		return getNext();
	} else if (ent->d_type != DT_REG)
	{
		return getNext();
	} else
	{
		optional<RDE_Item> ret;
		ret = RDE_Item{pimpl->path, ent->d_name};
		return ret;
	}
}

RDE::~RDE() = default;

#include "Windows.h"

HANDLE                     hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO csbi;

void getinit()
{
	static bool first = true;
	if (!first)
		return;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	first = false;
}

void MakeRed()
{
	getinit();
	SetConsoleTextAttribute(hConsole, 12);
}

void MakeGreen()
{
	getinit();
	SetConsoleTextAttribute(hConsole, 10);
}

void MakeHighlight()
{
	getinit();
	SetConsoleTextAttribute(hConsole, 15);
}

void MakeNormal() { SetConsoleTextAttribute(hConsole, csbi.wAttributes); }
