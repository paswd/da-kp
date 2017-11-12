#ifndef MUSICLIB_H
#define MUSICLIB_H

#include <iostream>
#include <map>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <string>
#include <mpg123.h>
#include "tmpg123.h"

class TMusicLib {
private:
	std::vector <std::string> Files(0);
	std::map <size_t, std::vector <std::pair<size_t, size_t>>> Lib(0);
	TMpg123 Mh;

public:
	void Build(char *filename);
	void Export(char *filename);
	void Import(char *filename);
	std::string Check(char *filename);
};

#endif //MUSICLIB_H
