#ifndef READ_H
#define READ_H

#include <iostream>
#include <vector>
#include "tmpg123.h"
#include "types.h"

//void ProcessFile(std::vector <float> *arr, mpg123_handle *mh, const char* file);
void ReadSingleMP3(char *name, std::vector <TSize> *out, TMpg123 *mh);

#endif //READ_H
