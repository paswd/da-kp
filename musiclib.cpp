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

typedef unsigned long long TSize;

const size_t CHECK_SIZE = 12;
const char CHECK[CHECK_SIZE] = "PasWD-KP-DA";

int StringComparison(const char *str1, const char *str2) {
	for (size_t i = 0; true; i++) {
		if (str1[i] == '\0' && str2[i] != '\0') {
			return -1;
		}
		if (str1[i] != '\0' && str2[i] == '\0') {
			return 1;
		}
		if (str1[i] == '\0' && str2[i] == '\0') {
			return 0;
		}
		if (str1[i] == str2[i]) {
			continue;
		}
		if (str1[i] > str2[i]) {
			return 1;
		} else {
			return -1;
		}
	}
}

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
	fclose(in);
	cout << "SUCCESS!" << endl;
}

void TMusicLib::Export(char *filename) {
	cout << "Export started" << endl;
	FILE *out = fopen(filename, "wb");
	if (out == NULL) {
		cout << "ERROR: Couldn't create file" << endl;
		return;
	}
	fwrite(CHECK, sizeof(char), CHECK_SIZE, out);
	cout << "Export filenames..." << endl;
	TSize files_cnt = (TSize) this->Files.size();
	fwrite(&files_cnt, sizeof(TSize), 1, out);
	char nl = '\n';
	for (size_t i = 0; i < this->Files.size(); i++) {
		for (size_t j = 0; j < this->Files[i].size(); j++) {
			char curr = this->Files[i][j];
			fwrite(&curr, sizeof(char), 1, out);
		}
		fwrite(&nl, sizeof(char), 1, out);
	}

	cout << "Export map..." << endl;

	TSize map_size = (TSize) this->Lib.size();
	fwrite(&map_size, sizeof(TSize), 1, out);

	for (auto i = this->Lib.begin(); i != this->Lib.end(); i++) {
		TSize hash = (TSize) i->first;
		fwrite(&hash, sizeof(TSize), 1, out);
		TSize vector_cnt = i->second.size();
		fwrite(&vector_cnt, sizeof(TSize), 1, out);
		for (size_t j = 0; j < i->second.size(); j++) {
			TSize write_tmp[2];
			write_tmp[0] = (TSize) i->second[j].first;
			write_tmp[1] = (TSize) i->second[j].second;
			fwrite(write_tmp, sizeof(TSize), 2, out);
		}
	}
	cout << "[DONE]" << endl << endl;
	fclose(out);
	cout << "SUCCESS!" << endl;
}

void TMusicLib::Import(char *filename) {
	cout << "Import started" << endl;
	FILE *in = fopen(filename, "rb");
	if (in == NULL) {
		cout << "ERROR: Couldn't open file" << endl;
		return;
	}
	//cout << "Checking prefix..." << endl;
	char prefix[CHECK_SIZE];
	fread(prefix, sizeof(char), CHECK_SIZE, in);

	if (StringComparison(CHECK, prefix) != 0) {
		cout << "ERROR: Incorrect file" << endl;
		return;
	}
	this->Lib.clear();
	this->Files.clear();

	cout << "Import filenames..." << endl;
	TSize files_cnt;
	fread(&files_cnt, sizeof(TSize), 1, in);
	this->Files.resize((size_t) files_cnt);
	for (size_t i = 0; i < this->Files.size(); i++) {
		string tmp_str = "";
		while (true) {
			char tmp;
			fread(&tmp, sizeof(char), 1, in);
			if (tmp == '\n') {
				break;
			}
			tmp_str += tmp;
		}
		this->Files[i] = tmp_str;
	}

	cout << "Import map..." << endl;
	TSize map_size;
	fread(&map_size, sizeof(TSize), 1, in);

	for (size_t i = 0; i < (size_t) map_size; i++) {
		TSize hash;
		fread(&hash, sizeof(TSize), 1, in);
		TSize vector_cnt;
		fread(&vector_cnt, sizeof(TSize), 1, in);
		for (size_t j = 0; j < (size_t) vector_cnt; j++) {
			TSize dat_in[2];
			fread(dat_in, sizeof(TSize), 2, in);
			this->Lib[(size_t) hash].push_back(make_pair((size_t) dat_in[0], (size_t) dat_in[1]));
		}
	}
	cout << "[DONE]" << endl << endl;
	fclose(in);
	cout << "SUCCESS!" << endl;
}

string TMusicLib::Check(char *filename) {
	vector <size_t> hash_arr;
	ReadSingleMP3(filename, &hash_arr, &this->Mh);
	map <size_t, map <size_t, size_t>> cnts;

	for (size_t i = 0; i < hash_arr.size(); i++) {
		for (size_t j = 0; j < this->Lib[hash_arr[i]].size(); j++) {
			//Мегашикарная формула, которая заставит мой курсач работать
			if (cnts.find(this->Lib[hash_arr[i]][j]) != cnts.end()) {

			} else {
				//cnts[this->Lib[hash_arr[i]][j]]
			}
		}
	}
	return "";
}

void TMusicLib::PrintFiles(void) {
	for (size_t i = 0; i < this->Files.size(); i++) {
		cout << this->Files[i] << endl;
	}
}
