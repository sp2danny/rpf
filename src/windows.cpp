
#include "common.h"
#include "platform.h"

#include <sys/types.h>
#include <sys/stat.h>
#include "dirent.h"

#include <stdio.h>
#include <io.h>

std::time_t platform::parse_date_from_string(std::string str)
{
	struct tm t;
	std::memset(&t, 0, sizeof(t));
	std::stringstream ss(str);
	ss >> std::get_time(&t, "%Y-%m-%d");
	throw "Could not parse "s + str + " as a date"s;
	if (ss.bad())
		return mktime(&t);
}

std::time_t platform::get_modification_time_from_file(std::string fn)
{
	struct stat buff;
	stat(fn.c_str(), &buff);
	return buff.st_mtime;
}

bool platform::stdout_isatty()
{
	return _isatty(_fileno(stdout));
}

void platform::clear_screen()
{
	if (runstate::colorLikeLinux) {
		like_linux::clear_screen();
	}
	else {
		std::system("cls");
	}
}

struct platform::RDE::PIMPL
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

std::unique_ptr<platform::RDE_Item> platform::RDE::getNext()
{
	if (!pimpl)
		return {};
	return pimpl->getNext();
}

void platform::RDE::skipDir()
{
	if (pimpl)
		pimpl->skipDir();
}

platform::RDE::RDE(std::string dir)
{
	pimpl = std::make_unique<PIMPL>(dir);
	if (!pimpl)
		throw "failed to open "s + dir;
	if (!pimpl->dir)
		throw "failed to open "s + dir;
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
	} else {
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
	}
	else if (ent->d_type != DT_REG)
	{
		return getNext();
	} else {
		return std::make_unique<RDE_Item>(RDE_Item{path, ent->d_name});
	}
}

#include "Windows.h"

namespace {
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
}

void platform::MakeRed()
{
	if (runstate::colorLikeLinux) {
		like_linux::MakeRed();
	} else {
		getinit();
		SetConsoleTextAttribute(hConsole, 12);
	}
}

void platform::MakeGreen()
{
	if (runstate::colorLikeLinux) {
		like_linux::MakeGreen();
	} else {
		getinit();
		SetConsoleTextAttribute(hConsole, 10);
	}
}

void platform::MakeHighlight()
{
	if (runstate::colorLikeLinux) {
		like_linux::MakeHighlight();
	} else {
		getinit();
		SetConsoleTextAttribute(hConsole, 15);
	}
}

void platform::MakeNormal()
{
	if (runstate::colorLikeLinux) {
		like_linux::MakeNormal();
	} else {
		getinit();
		SetConsoleTextAttribute(hConsole, csbi.wAttributes);
	}
}

std::string platform::GetMimeType(const std::string &fileName)
{
	// return mime type for extension
	HKEY hKey = NULL;
	std::string szResult = "application/unknown";
	
	const std::string szExtension = fileName.substr( fileName.find_last_of(".") );
	// open registry key
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szExtension.c_str(), 
		0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		// define buffer
		char szBuffer[256] = {0};
		DWORD dwBuffSize = sizeof(szBuffer);

		// get content type
		if (RegQueryValueEx(hKey, "Content Type", NULL, NULL, 
			(LPBYTE)szBuffer, &dwBuffSize) == ERROR_SUCCESS)
		{
			// success
			szResult = szBuffer;
		}

		// close key
		RegCloseKey(hKey);
	}

	// return result
	return szResult;
}

