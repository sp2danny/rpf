
#pragma once

#include "common.h"

#define MAKE_OPER( xx )                                                \
struct xx ## Operator final : Operator                                 \
{                                                                      \
	xx ## Operator          ( std::string str       ) { Create(str); } \
	virtual char MyChar     (                       ) override;        \
	virtual void Create     ( std::string           ) override;        \
	virtual void MatchDir   ( File&, FileMatchStack ) override;        \
	virtual void MatchFile  ( File&, FileMatchStack ) override;        \
	virtual void MatchLines ( File&, LineMatchStack ) override;        \
};

MAKE_OPER( File );
MAKE_OPER( Dir  );
MAKE_OPER( Line );
MAKE_OPER( And  );
MAKE_OPER( Or   );


#undef MAKE_OPER


