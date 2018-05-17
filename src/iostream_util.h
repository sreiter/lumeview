#ifndef __H__iostream_util
#define __H__iostream_util

#include <vector>
#include <iostream>

template<typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
    out << "{";
    for(size_t i = 0; i + 1 < v.size(); ++i) {
        out << v[i];
        if (i + 1 != v.size()) 
            out << ", ";
    }
    out << "}";
    return out;
}

#endif	//__H__iostream_util
