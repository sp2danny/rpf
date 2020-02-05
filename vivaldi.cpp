
#include <set>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <vector>
#include <cmath>
#include <limits>

// ------------------------------------------------------------------------------------

// ******************
// *** SortedInts ***
// ******************

using myInt = long long;

class SortedInts
{
public:
	int size() const;
	myInt nth(int) const;
	bool add(myInt);
	bool have(myInt) const;
	int pers() const;
private:
	std::set<myInt> mInts;
};

int SortedInts::pers() const
{
	auto iter = mInts.end();
	--iter;
	myInt maxval = *iter;
	float f = (float)maxval;
	f /= std::numeric_limits<myInt>::max();
	f *= 100;
	return (int)f;
}

int SortedInts::size() const
{
	return static_cast<int>(mInts.size());
}

myInt SortedInts::nth(int n) const
{
	assert(n <= size());
	assert(n >= 1);

	auto iter = mInts.begin();
	std::advance(iter, n - 1);

	return *iter;
}

bool SortedInts::add(myInt i)
{
	auto res = mInts.insert(i);
	return res.second;
}

bool SortedInts::have(myInt i) const
{
	return mInts.find(i) != mInts.end();
}

// ------------------------------------------------------------------------------------

// ********************
// *** Generate3sum ***
// ********************

struct int3
{
	int a, b, c;
};

// generate all combination of 3 non-negative integers whose sum is n
std::vector<int3> generate3sum(int n)
{
	std::vector<int3> res;
	for (int a = 0; a <= n; ++a)
	{
		for (int b = 0; b <= (n - a); ++b)
		{
			int c = n - (a + b);
			res.push_back({ a, b, c });
		}
	}
	return res;
}

// ------------------------------------------------------------------------------------

long long ipow(int base, int exp)
{
	long long result = 1;
	long long mybase = base;
	for (;;)
	{
		if (exp & 1)
			result *= mybase;
		exp >>= 1;
		if (!exp)
			break;
		mybase *= mybase;
	}

	return result;
}

// ------------------------------------------------------------------------------------

// ***************
// *** Vivaldi ***
// ***************

constexpr int BitLimt = 60; // a bit arbitrary, if the sought-after number have more bits, algo will fail

class Vivaldi
{
public:
	void generatePass(int);
	myInt lastLowest() const { return mLastPassLowest; }
	std::pair<bool, myInt> candidateN(int) const;
	int pers() const { return mCandidates.pers(); }
private:
	myInt mLastPassLowest;
	SortedInts mCandidates;
};

void Vivaldi::generatePass(int n)
{
	auto vec3 = generate3sum(n);

	auto powN = [](const int3& i3) -> myInt {
		auto p1l = i3.a * std::log2(2);
		if (p1l >= BitLimt) return 0;
		long long p1 = ipow(2, i3.a);

		auto p2l = i3.b * std::log2(3);
		if (p2l >= BitLimt) return 0;
		long long p2 = ipow(3, i3.b);

		auto p3l = i3.c * std::log2(5);
		if (p3l >= BitLimt) return 0;
		long long p3 = ipow(5, i3.c);

		if ((p1l+p2l+p3l) >= BitLimt)
			return 0;

		return (myInt)(p1*p2*p3);
	};

	bool haveLowest = false;
	myInt currentLowest;

	for (const auto& i3 : vec3)
	{
		myInt cand = powN(i3);
		if (!cand) continue;
		if (!haveLowest)
		{
			currentLowest = cand;
			haveLowest = true;
		}
		else if (cand < currentLowest)
		{
			currentLowest = cand;
		}

		[[maybe_unused]]
		bool added;

		added = mCandidates.add(cand);
		assert(added);
	}

	assert(haveLowest);
	mLastPassLowest = currentLowest;
}

std::pair<bool, myInt> Vivaldi::candidateN(int n) const
{
	if (n > mCandidates.size()) return {false, 0};
	myInt cand = mCandidates.nth(n);
	return {true, cand};
}

// ------------------------------------------------------------------------------------

int main()
{
	Vivaldi viv;

	viv.generatePass(0);

	int i = 1;
	int pass = 1;

	while (i <= 10'000)
	{
		while (true)
		{
			myInt ll = viv.lastLowest();
			auto cand = viv.candidateN(i);
			if (cand.first && (cand.second < ll))
			{
				std::cout << i << " : " << cand.second << " (" << pass << ") (" << viv.pers() << "%)\n";
				break;
			}
			viv.generatePass(pass++);
		}
		++i;
	}

	fgetc(stdin);
	std::cout << "\ndone." << std::endl;
}





