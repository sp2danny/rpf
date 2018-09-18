
#pragma once

#include "common.h"

extern bool Execute_BM       (Results&, const Operator&);
extern bool Execute_BMCI     (Results&, const Operator&);
extern bool Execute_Invert   (Results&, const Operator&);
extern bool Execute_Swap     (Results&, const Operator&);
extern bool Execute_Ident    (Results&, const Operator&);
extern bool Execute_IdentCI  (Results&, const Operator&);
extern bool Execute_Sub      (Results&, const Operator&);
extern bool Execute_True     (Results&, const Operator&);
extern bool Execute_Dir      (Results&, const Operator&);
extern bool Execute_CppOnly  (Results&, const Operator&);
extern bool Execute_File     (Results&, const Operator&);
extern bool Execute_Line     (Results&, const Operator&);
extern bool Execute_LineCI   (Results&, const Operator&);
extern bool Execute_Regexp   (Results&, const Operator&);
extern bool Execute_Bail     (Results&, const Operator&);
extern bool Execute_Pop      (Results&, const Operator&);
extern bool Execute_Negate   (Results&, const Operator&);
extern bool Execute_Dup      (Results&, const Operator&);
extern bool Execute_And      (Results&, const Operator&);
extern bool Execute_Or       (Results&, const Operator&);
extern bool Execute_Near     (Results&, const Operator&);
extern bool Execute_Clear    (Results&, const Operator&);
extern bool Execute_Mod      (Results&, const Operator&);

