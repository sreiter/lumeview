
#include <fstream>
#include <string>
#include "file_util.h"
#include "cond_throw.h"

using namespace std;

namespace lumeview {

string LoadStringFromFile (const char* filename)
{
	ifstream t(filename);
	COND_THROW (!t, "FILE::LOAD_FROM_STRING" << endl <<
	            "Couldn't find file " << filename);
	t.seekg(0, ios::end);
	size_t size = t.tellg();
	string buffer(size, ' ');
	t.seekg(0);
	t.read(&buffer[0], size);
	return std::move(buffer);
}

string LoadStringFromFile (const std::string& filename)
{
	return LoadStringFromFile (filename.c_str());
}

}// end of namespace lumeview