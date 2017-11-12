#include "musiclib.h"

#include <iostream>
#include <map>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <mpg123.h>

using namespace std;

void TMusicLib::Build(char *filename) {
	//Get filenames from file
	FILE *in = fopen(filename, "r");

	if (in == NULL) {
		cout << "ERROR: Couldn't open file" << endl;
		return;
	}
	char tmp;
	string tmp_str = "";

	while (fscanf(in, "%c", &tmp) != EOF) {
		if (tmp == '\n') {
			if (!tmp_str.empty) {
				this->Files.push_back(tmp_str);
			}
			tmp_str = "";
			continue;
		}
		tmp_str += tmp;
	}
	if (!tmp_str.empty) {
		this->Files.push_back(tmp_str);
	}

	//Building map
	vector <size_t> hash_arr;

	for (size_t i = 0; i < this->Files.size(); i++) {
		ReadSingleMP3(this->Files[i].c_str(), &hash_arr, &this->Mh);
		for (size_t j = 0; j < hash_arr.size(); j++) {
			this->Lib[hash_arr[j]].push_back(make_pair(i, j));
		}
	}
}

void TMusicLib::Export(char *filename) {
	
}

void TMusicLib::Import(char *filename) {
	
}

string TMusicLib::Check(char *filename) {
	
}
