
#include <string>

#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
	
#include "common.h"
#include "iniparser.h"

void recursiveLoad(IniFile& ini, const std::string& fn)
{
	auto dir = fs::current_path();
	while (true)
	{
		auto ifn = dir / fn;
		if (fs::exists(ifn))
		{
			IniFile i;
			i.LoadFile(ifn.generic_string());
			auto headers = i.ListHeaders();
			for (auto&& hdr : headers)
			{
				if (ini.HasHeader(hdr))
				{
					auto keys = i.ListKeys(hdr);
					for (auto&& k : keys)
					{
						if (!ini.HasKey(hdr, k))
							ini.all[hdr][k] = i.all[hdr][k];
					}
				}
				else {
					ini.all[hdr] = i.all[hdr];
				}
			}
		}
		if (!dir.has_parent_path()) break;
		if (dir == dir.parent_path()) break;
		dir = dir.parent_path();
	}
}

