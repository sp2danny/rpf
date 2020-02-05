
#include "Vivaldi2.h"

long long Vivaldi2::getNext()
{
	auto iter = mCurrent.begin();
	long long value = *iter;
	mCurrent.erase(iter);
	mCurrent.insert(value*2);
	mCurrent.insert(value*3);
	mCurrent.insert(value*5);
	return value;
}

