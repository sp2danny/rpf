
#pragma once

#include "common.h"

#define MAKE_OPER_NORM( xx )                                            \
struct xx ## Operator final : Operator                                  \
{                                                                       \
	xx ## Operator          ( std::string str        ) { Create(str); } \
	virtual char MyChar     (                        ) override;        \
	virtual void Create     ( std::string            ) override;        \
	virtual void MatchFile  ( File&, FileMatchStack& ) override;        \
	virtual void MatchLines ( File&, LineMatchStack& ) override;        \
	virtual void Register   (                        ) override         \
	{ auto maker = [](std::string s) -> clone_ptr<Operator> {           \
	  auto o = make_cloned<xx ## Operator>(s);                          \
	  return clone_ptr<Operator>(o); };                                 \
	  Operator::Register( MyChar() , +maker ); }                        \
	virtual Operator* clone (                        ) override         \
	{ return new xx ## Operator(*this); }                               \
private:                                                                \
	xx ## Operator() = default;                                         \
	friend void register_all();


#define MAKE_OPER_FILE( xx )                                            \
struct xx ## Operator final : Operator                                  \
{                                                                       \
	xx ## Operator          ( std::string str        ) { Create(str); } \
	virtual char MyChar     (                        ) override;        \
	virtual void Create     ( std::string            ) override;        \
	virtual void MatchFile  ( File&, FileMatchStack& ) override;        \
	virtual void MatchLines ( File&, LineMatchStack& ) override;        \
	virtual void LinesCache ( File&                  ) override;        \
	virtual bool IsCached   (                        ) override;        \
	virtual int  MyPrio     (                        ) override;        \
	virtual void ExeCached  ( LineMatchStack&        ) override;        \
	virtual void Register   (                        ) override         \
	{ auto maker = [](std::string s) -> clone_ptr<Operator> {           \
	  auto o = make_cloned<xx ## Operator>(s);                          \
	  return clone_ptr<Operator>(o); };                                 \
	  Operator::Register( MyChar() , +maker ); }                        \
	virtual Operator* clone (                        ) override         \
	{ return new xx ## Operator(*this); }                               \
private:                                                                \
	xx ## Operator() = default;                                         \
	friend void register_all();                                         \
	LineMatch lm_cache;                                                 \
    bool have_cache = false;                                            \

typedef clone_ptr<boyer_moore> pBM;

typedef clone_ptr<boyer_moore_ci> pBMCI;

typedef clone_ptr<std::regex> pRE;

MAKE_OPER_NORM( File    )     std::string name; };
MAKE_OPER_NORM( CppOnly )     std::string name; };
MAKE_OPER_NORM( Dir     )     std::string name; };
MAKE_OPER_FILE( Line    )     std::string expr; };
MAKE_OPER_NORM( LineCI  )     std::string expr; };
MAKE_OPER_NORM( And     )     bool all;         };
MAKE_OPER_NORM( Or      )     bool all;         };
MAKE_OPER_NORM( Not     )                       };
MAKE_OPER_NORM( Near    )     UL n;             };
MAKE_OPER_NORM( BM      )     pBM bm; UL sz;    };
MAKE_OPER_NORM( BM_CI   )     pBMCI bm; UL sz;  };
MAKE_OPER_NORM( Regex   )     pRE re;           };
MAKE_OPER_NORM( Sub     )                       };
MAKE_OPER_NORM( True    )                       };
MAKE_OPER_NORM( Inv     )                       };
MAKE_OPER_NORM( Mod     )     std::time_t md;   };
MAKE_OPER_NORM( Ident   )     std::string id;   };
MAKE_OPER_NORM( IdentCI )     std::string id;   };
MAKE_OPER_NORM( Dup     )                       };
MAKE_OPER_NORM( Swap    )     UL pos;           };


#undef MAKE_OPER


