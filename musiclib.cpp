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
#include "read.h"

using namespace std;

TMusicLib::TMusicLib(void) {
	this->Files.clear();
	this->Lib.clear();
}

void TMusicLib::Build(char *filename) {
	//Get filenames from file
	FILE *in = fopen(filename, "r");

	if (in == NULL) {
		cout << "ERROR: Couldn't open file" << endl;
		return;
	}
	char tmp;
	string tmp_str = "";

	cout << "Read filenames... ";
	//size_t cnt = 0;
	//cout << cnt << endl;
	while (fscanf(in, "%c", &tmp) != EOF) {
		if (tmp == '\n') {
			if (!tmp_str.empty()) {
				//cout << tmp_str << endl;
				this->Files.push_back(tmp_str);
			}
			tmp_str = "";
			continue;
		}
		tmp_str += tmp;
	}
	if (!tmp_str.empty()) {
		this->Files.push_back(tmp_str);
	}

	cout << "[DONE]" << endl;
	cout << "Detected " << this->Files.size() << " filenames" << endl << endl;
	cout << "Building library" << endl;
	//Building map
	vector <size_t> hash_arr;

	for (size_t i = 0; i < this->Files.size(); i++) {
		//cout << i << endl;
		cout << "Adding " << this->Files[i] << endl;
		ReadSingleMP3(strdup(this->Files[i].c_str()), &hash_arr, &this->Mh);
		for (size_t j = 0; j < hash_arr.size(); j++) {
			this->Lib[hash_arr[j]].push_back(make_pair(i, j));
		}
	}
	cout << "[DONE]" << endl << endl;
	cout << "SUCCESS!" << endl;
}

void TMusicLib::Export(char *filename) {
	
}

void TMusicLib::Import(char *filename) {
	
}

string TMusicLib::Check(char *filename) {
	return "";
}
