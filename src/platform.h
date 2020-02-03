
#pragma once

#include "common.h"

namespace platform
{

	extern void MakeRed();
	extern void MakeGreen();
	extern void MakeHighlight();
	extern void MakeNormal();

	extern std::string GetMimeType(const std::string&);

	struct RDE_Item
	{
		std::string dirName;
		std::string fileName;
	};

	struct RDE
	{
		RDE() = default;
		RDE(std::string dir);
		RDE(std::istream&);
		~RDE();

		RDE& operator=(RDE&&) noexcept = default;
		RDE(RDE&&) noexcept = default;

		//void swap(RDE& other) noexcept { pimpl.swap(other.pimpl); }

		std::unique_ptr<RDE_Item> getNext();
		void skipDir();

	private:
		struct PIMPL;
		typedef std::unique_ptr<PIMPL> UPIMPL;
		UPIMPL pimpl;
	};

	extern std::time_t parseDateFromString(std::string);
	extern std::time_t getModificationTimeFromFile(std::string);
	extern bool        stdoutIsatty();
	extern void        clearScreen();

}

namespace like_linux
{
	extern void clearScreen();
	extern void MakeHighlight();
	extern void MakeNormal();
	extern void MakeRed();
	extern void MakeGreen();
}
