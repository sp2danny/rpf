
#pragma once

#include <set>

class Vivaldi2
{
public:
	long long getNext();
private:
	std::set<long long> mCurrent = {1};
};


