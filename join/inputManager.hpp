#include <fcntl.h>      // for O_RDONLY
#include <unistd.h>     // for open/close
#include <stdint.h>     // for uint64_t

#include "stringList.hpp"

char * getFilePath(char * line);
char ** getInputFiles(uint64_t * size);
