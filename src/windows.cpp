
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
	std::string path;
	DIR* dir = nullptr;
	UPIMPL next;
	PIMPL(std::string path) : path(path), dir(opendir(path.c_str())) {}
	~PIMPL()
	{
		if (dir)
			closedir(dir);
	}
	// immobile only type
	PIMPL(const PIMPL&) = delete;
	PIMPL& operator=(const PIMPL&) = delete;

	std::unique_ptr<RDE_Item> getNext();
	void skipDir();

private:
	PIMPL() = default;
};

std::unique_ptr<RDE_Item> RDE::getNext()
{
	if (!pimpl)
		return {};
	return pimpl->getNext();
}

void RDE::skipDir()
{
	if (pimpl)
		pimpl->skipDir();
}

RDE::RDE(std::string dir)
{
	pimpl = std::make_unique<PIMPL>(dir); // new PIMPL(dir);
	if (!pimpl)
		throw "failed to open "s + dir;
	if (!pimpl->dir)
		throw "failed to open "s + dir;
}

RDE::~RDE() = default;

void RDE::PIMPL::skipDir()
{
	PIMPL* lst = nullptr;
	PIMPL* ptr = this;
	while (true)
	{
		if (ptr->next)
		{
			lst = ptr;
			ptr = ptr->next.get();
		} else
		{
			break;
		}
	}
	if (!lst)
	{
		if (dir)
			closedir(dir);
		dir = nullptr;
	} else
	{
		lst->next = nullptr;
	}
}

std::unique_ptr<RDE_Item> RDE::PIMPL::getNext()
{
	if (next)
	{
		auto ret = next->getNext();
		if (ret)
			return ret;
		next = nullptr;
		// pimpl->next.reset();
	}
	if (!dir)
		return {};
	dirent* ent = readdir(dir);
	if (!ent)
		return {};
	if (ent->d_type == DT_DIR)
	{
		if (ent->d_name[0] != '.')
			next = std::make_unique<PIMPL>(path + "/"s + ent->d_name);
		return getNext();
	} else if (ent->d_type != DT_REG)
	{
		return getNext();
	} else
	{
		return std::make_unique<RDE_Item>(RDE_Item{path, ent->d_name});
	}
}

#include "Windows.h"

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
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
