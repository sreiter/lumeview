#ifndef __H__file_util
#define __H__file_util

#include <string>

namespace lumeview {

std::string LoadStringFromFile (const char* filename);
std::string LoadStringFromFile (const std::string& filename);

}// end of namespace lumeview

#endif	//__H__file_util
