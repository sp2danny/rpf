
#pragma once

#include "common.h"
#include "OFM.h"


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
    virtual void UnCache    (                        ) override;        \
	virtual bool NeedFile   (                        ) override         \
	{ return true; }                                                    \
private:                                                                \
	xx ## Operator() = default;                                         \
	friend void register_all();                                         \
	LineMatch lm_cache;                                                 \
    bool have_cache = false;                                            \

struct boyer_moore;
typedef clone_ptr<boyer_moore> pBM;

template<typename Ch>
using ChEqP = bool (*)(Ch, Ch);
template<typename Ch, typename Str, ChEqP<Ch> >
struct boyer_moore_advanced;
extern bool isEqualNoCase(char, char);
typedef boyer_moore_advanced<char, std::string, isEqualNoCase> boyer_moore_ci;

typedef clone_ptr<boyer_moore_ci> pBMCI;

typedef clone_ptr<std::regex> pRE;

MAKE_OPER_NORM( File    )     std::string name;                };
MAKE_OPER_NORM( CppOnly )     std::string name;                };
MAKE_OPER_NORM( Dir     )     std::string name;                };
MAKE_OPER_FILE( Line    )     std::string expr; int prio;      };
MAKE_OPER_FILE( LineCI  )     std::string expr; int prio;      };
MAKE_OPER_NORM( And     )     UL cnt;                          };
MAKE_OPER_NORM( Or      )     UL cnt;                          };
MAKE_OPER_NORM( Not     )                                      };
MAKE_OPER_NORM( Near    )     UL n; bool all = false;          };
MAKE_OPER_FILE( BM      )     pBM bm; UL sz;                   };
MAKE_OPER_FILE( BM_CI   )     pBMCI bm; UL sz;                 };
MAKE_OPER_FILE( Regex   )     pRE re;                          };
MAKE_OPER_NORM( Sub     )                                      };
MAKE_OPER_NORM( True    )                                      };
MAKE_OPER_NORM( Inv     )                                      };
MAKE_OPER_NORM( Mod     )     std::time_t md;                  };
MAKE_OPER_FILE( Ident   )     std::string id;                  };
MAKE_OPER_FILE( IdentCI )     std::string id;                  };
MAKE_OPER_NORM( Dup     )                                      };
MAKE_OPER_NORM( Swap    )     UL pos;                          };
MAKE_OPER_NORM( Range   )     UL max;                          };
MAKE_OPER_FILE( Text    )     bool res;                        };



#undef MAKE_OPER


