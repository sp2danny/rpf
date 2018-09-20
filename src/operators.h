
#pragma once

#include "common.h"

#define MAKE_OPER( xx )                                                 \
struct xx ## Operator final : Operator                                  \
{                                                                       \
	xx ## Operator          ( std::string str        ) { Create(str); } \
	virtual char MyChar     (                        ) override;        \
	virtual void Create     ( std::string            ) override;        \
	virtual void MatchDir   ( File&, FileMatchStack& ) override;        \
	virtual void MatchFile  ( File&, FileMatchStack& ) override;        \
	virtual void MatchLines ( File&, LineMatchStack& ) override;        \
	virtual void Register   (                        ) override;        \
	virtual Operator* clone (                        ) override;        \
private:                                                                \
	xx ## Operator() = default;                                         \
	friend void register_all();


typedef clone_ptr<boyer_moore> pBM;

typedef clone_ptr<boyer_moore_ci> pBMCI;

typedef clone_ptr<std::regex> pRE;

MAKE_OPER( File    )     std::string name; };
MAKE_OPER( CppOnly )     std::string name; };
MAKE_OPER( Dir     )     std::string name; };
MAKE_OPER( Line    )     std::string expr; };
MAKE_OPER( LineCI  )     std::string expr; };
MAKE_OPER( And     )                       };
MAKE_OPER( Or      )                       };
MAKE_OPER( Not     )                       };
MAKE_OPER( Near    )     UL n;             };
MAKE_OPER( BM      )     pBM bm; UL sz;    };
MAKE_OPER( BM_CI   )     pBMCI bm; UL sz;  };
MAKE_OPER( Regex   )     pRE re;           };

#undef MAKE_OPER


