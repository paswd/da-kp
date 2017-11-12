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
	std::vector <std::string> Files;
	std::map <size_t, std::vector <std::pair<size_t, size_t>>> Lib; //(id, time)
	TMpg123 Mh;

public:
	TMusicLib(void);
	void Build(char *filename);
	void Export(char *filename);
	void Import(char *filename);
	std::string Check(char *filename);

	void PrintFiles(void);
};

#endif //MUSICLIB_H
