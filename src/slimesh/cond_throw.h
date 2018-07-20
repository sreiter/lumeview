#ifndef __H__slimesh_cond_throw
#define __H__slimesh_cond_throw

#include <exception>
#include <sstream>
// #include "iostream_util.h"

#define THROW(msg)	{std::stringstream ss; ss << msg; throw(std::runtime_error(ss.str()));}
#define COND_THROW(cond, msg)	if(cond){THROW(msg);}

#endif	//__H__slimesh_cond_throw
