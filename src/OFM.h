
// Operator File Match

#pragma once

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
	LineMatch(bool m, const Lines& l) : m_match(FromBool(m)), m_lines(l) {}
	LineMatch(bool m, Lines&& l) : m_match(FromBool(m)), m_lines(std::move(l)) {}
	LineMatch(TriBool m) : m_match(m) {}
	bool match() const;
	TriBool tri() const { return m_match; }
	void match(bool);
	const Lines& lines() const;
	Lines& modifiable_lines();
	void add_simple_match(UL l);
	void add_full_match(UL l, UL b, UL e);
	void add_full_match(UL l, MIP mip);
	void add_full_match(LIter li, UL b, UL e);
	void add_full_match(LIter li, MIP mip);
	void add_all_matches(UL l, const MMIP&);
	bool have_line(UL) const;
	bool have_char(UL,UL) const;
	bool have_char(LCIter,UL) const;
private:
	TriBool m_match = tb_false;
	Lines m_lines;
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

protected:
	File* file = nullptr;

private:
	static std::map<char, OperatorMaker> createMap;
};

typedef std::vector<clone_ptr<Operator>> OperatorStack;

extern TriBool ExecuteAllFile ( OperatorStack&, FileMatchStack& );
extern bool    ExecuteAllLine ( OperatorStack&, LineMatchStack& );

struct File
{
	std::string path;
	std::string name;
	std::vector<std::string>& lines();
	bool cpponly;
private:
	std::vector<std::string> m_lines;
	std::vector<std::string> m_stripped;
	bool m_loaded = false;
};
