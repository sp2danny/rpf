
#pragma once

// Operator File Match

#include "common.h"
#include "clone_ptr.hpp"

typedef std::pair<UL, UL> MIP;
typedef std::vector<MIP> MMIP;
typedef std::map<UL, MMIP> Lines;
typedef Lines::iterator LIter;
typedef Lines::const_iterator LCIter;

struct LineMatch
{
	LineMatch() = default;
	LineMatch(bool m, const Lines& l) : mMatch(FromBool(m)), mLines(l) {}
	LineMatch(bool m, Lines&& l) : mMatch(FromBool(m)), mLines(std::move(l)) {}
	LineMatch(TriBool m) : mMatch(m) {}
	bool match() const;
	TriBool tri() const { return mMatch; }
	void match(bool);
	const Lines& lines() const;
	Lines& modifiableLines();
	void addSimpleMatch(UL l);
	void addFullMatch(UL l, UL b, UL e);
	void addFullMatch(UL l, MIP mip);
	void addFullMatch(LIter li, UL b, UL e);
	void addFullMatch(LIter li, MIP mip);
	void addAllMatches(UL l, const MMIP&);
	bool haveLine(UL) const;
	bool haveChar(UL, UL) const;
	bool haveChar(LCIter, UL) const;
private:
	TriBool mMatch = TriBool::False;
	Lines mLines;
};

typedef std::vector<LineMatch> LineMatchStack;

typedef std::vector<TriBool> FileMatchStack;

struct Operator;

typedef clone_ptr<Operator> (*OperatorMaker)(std::string);

struct File;

struct Operator
{
	Operator() = default;
	virtual ~Operator() = default;
	virtual char MyChar() = 0;
	virtual void Create(std::string) = 0;
	virtual Operator* clone() = 0;

	static void Register(char, OperatorMaker);
	static clone_ptr<Operator> DispatchCreate(std::string);

	virtual void Register() = 0;

	virtual void MatchFile  ( File&, FileMatchStack& ) = 0;
	virtual void MatchLines ( File&, LineMatchStack& ) = 0;

	virtual void LinesCache ( File& f                ) { file = &f; }
	virtual bool IsCached   (                        ) { return file != nullptr; }
	virtual int  MyPrio     (                        ) { return 1; };
	virtual void ExeCached  ( LineMatchStack& lms    ) { MatchLines(*file, lms); }
	virtual void UnCache    (                        ) { file = nullptr; }
	virtual bool NeedFile   (                        ) { return false; }

	virtual void Print(std::ostream&) const = 0;

protected:
	File* file = nullptr;

private:
	static std::map<char, OperatorMaker> createMap;
};

inline std::ostream& operator<<(std::ostream& out, const clone_ptr<Operator>& oper)
{
	oper->Print(out);
	return out;
}

typedef std::vector<clone_ptr<Operator>> OperatorStack;

extern TriBool ExecuteAllFile(OperatorStack&, FileMatchStack&);
extern bool    ExecuteAllLine(OperatorStack&, LineMatchStack&);

extern void printStack(const FileMatchStack&);
extern void printStack(const LineMatchStack&);

struct File
{
	std::string path;
	std::string name;
	std::vector<std::string>& lines();
	bool cpponly;
private:
	std::vector<std::string> mLines;
	std::vector<std::string> mStripped;
	bool mLoaded = false;
};
