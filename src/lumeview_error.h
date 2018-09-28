#ifndef __H__cond_throw
#define __H__cond_throw

#include <sstream>
#include "lume/custom_exception.h"
#include "iostream_util.h"

namespace lumeview {
DECLARE_CUSTOM_EXCEPTION (LumeviewError, std::runtime_error);
}// end of namespace lumeview

#define THROW(msg)	{std::stringstream ss; ss << msg; throw(lumeview::LumeviewError(ss.str()));}
#define COND_THROW(cond, msg)	if(cond){THROW(msg);}

#endif	//__H__cond_throw
