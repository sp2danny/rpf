
#include "common.h"
#include "platform.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


bool platform::stdout_isatty() { return isatty(fileno(stdout)); }

std::time_t platform::parse_date_from_string(std::string str)
{
	struct tm t;
	std::memset(&t, 0, sizeof(t));
	int  y, m, d;
	auto f = std::sscanf(str.c_str(), "%04d-%02d-%02d", &y, &m, &d);
	if (f != 3)
		throw "Could not parse "s + str + " as a date"s;
	t.tm_year = y - 1900;
	t.tm_mon  = m - 1;
	t.tm_mday = d;
	return mktime(&t);
}

std::time_t platform::get_modification_time_from_file(std::string fn)
{
	struct stat buff;
	stat(fn.c_str(), &buff);
	return buff.st_mtime;
}

void clear_screen()   { like_linux::clear_screen(); }
void MakeHighlight()  { like_linux::MakeHighlight(); }
void MakeNormal()     { like_linux::MakeNormal(); }
void MakeRed()        { like_linux::MakeRed(); }
void MakeGreen()      { like_linux::MakeGreen(); }

struct platform::RDE::PIMPL
{
	std::string path;
	std::istream* in = nullptr;
	DIR* dir = nullptr;
	UPIMPL next;
	PIMPL(std::string path) : path(path), dir(opendir(path.c_str())) {}
	PIMPL(std::istream& in) : in(&in) {}
	~PIMPL() { if (dir) closedir(dir); }
	// immobile only type
	PIMPL(const PIMPL&) = delete;
	PIMPL& operator=(const PIMPL&) = delete;

	std::unique_ptr<RDE_Item> getNext();
	void skipDir();

private:
	PIMPL() = default;
};

std::unique_ptr<platform::RDE_Item> platform::RDE::getNext()
{
	if (!pimpl)
		return {};
	return
		pimpl->getNext();
}

void platform::RDE::skipDir()
{
	if (pimpl)
		pimpl->skipDir();
}

platform::RDE::RDE(std::string dir)
{
	pimpl = std::make_unique<PIMPL>(dir); // new PIMPL(dir);
	if (!pimpl)
		throw "failed to open "s + dir;
	if (!pimpl->dir)
		throw "failed to open "s + dir;
}

platform::RDE::RDE(std::istream& in)
{
	pimpl = std::make_unique<PIMPL>(in); // new PIMPL(dir);
	if (!pimpl)
		throw "failed to initialize "s;
}

platform::RDE::~RDE() = default;

void platform::RDE::PIMPL::skipDir()
{
	PIMPL* lst = nullptr;
	PIMPL* ptr = this;
	while (true)
	{
		if (ptr->next)
		{
			lst = ptr;
			ptr = ptr->next.get();
		}
		else
		{
			break;
		}
	}
	if (!lst)
	{
		if (dir) closedir(dir);
		dir = nullptr;
	}
	else
	{
		lst->next = nullptr;
	}
}

std::unique_ptr<platform::RDE_Item> platform::RDE::PIMPL::getNext()
{
	if (next)
	{
		auto ret = next->getNext();
		if (ret)
			return ret;
		next = nullptr;
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
	}
	else if (ent->d_type != DT_REG)
	{
		return getNext();
	}
	else
	{
		return std::make_unique<RDE_Item>(RDE_Item{path, ent->d_name});
	}
}

#include <magic.h>

std::string GetMimeType(const std::string& fn)
{
	const char *mime;
	static magic_t magic;
	static bool first = true;

	if (first)
	{
		magic = magic_open(MAGIC_MIME_TYPE); 
		magic_load(magic, nullptr);
		magic_compile(magic, nullptr);
		first = false;
	}
	mime = magic_file(magic, fn.c_str());
	return {mime};
}

