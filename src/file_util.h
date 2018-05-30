#ifndef __H__file_util
#define __H__file_util

#include <string>

namespace slimesh {

std::string LoadStringFromFile (const char* filename);
std::string LoadStringFromFile (const std::string& filename);

}// end of namespace slimesh

#endif	//__H__file_util
