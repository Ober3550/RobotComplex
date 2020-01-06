#include "MyMap.h"
off_t GetFileLength(std::string const& filename){
struct stat st;
if (stat(filename.c_str(), &st) != -1)
return st.st_size;
}