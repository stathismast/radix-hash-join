#include <fcntl.h>      // for O_RDONLY
#include <unistd.h>     // for open/close
#include <stdint.h>     // for uint64_t
#include <ctype.h>      // for isspace
#include <unistd.h>     // for access

#include "stringList.hpp"

char * getFilePath(char * line);
char ** getInputFiles(uint64_t * size);
bool fileExists(char * filePath);
bool ignoreLine(char * line);
