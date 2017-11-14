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
#include <algorithm>
#include "read.h"
#include "types.h"

using namespace std;

//typedef unsigned long long TSize;

const TSize CHECK_SIZE = 12;
const char CHECK[CHECK_SIZE] = "PasWD-KP-DA";
const TSize MIN_DIFF = 1024;
const string NF_MESSAGE = "! NOT FOUND";
const string MSG_DONE = "[DONE]";

int StringComparison(const char *str1, const char *str2) {
	for (TSize i = 0; true; i++) {
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
	//TSize cnt = 0;
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

	cout << MSG_DONE << endl;
	cout << "Detected " << this->Files.size() << " filenames" << endl << endl;
	cout << "Building library" << endl;
	//Building map
	vector <TSize> hash_arr;

	for (TSize i = 0; i < this->Files.size(); i++) {
		//cout << i << endl;
		cout << "Adding " << this->Files[i] << endl;
		ReadSingleMP3(strdup(this->Files[i].c_str()), &hash_arr, &this->Mh);
		for (TSize j = 0; j < hash_arr.size(); j++) {
			this->Lib[hash_arr[j]].push_back(make_pair(i, j));
		}
	}
	cout << MSG_DONE << endl << endl;
	cout << "Added " << this->Lib.size() << " hash notes" << endl;
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
	for (TSize i = 0; i < this->Files.size(); i++) {
		for (TSize j = 0; j < this->Files[i].size(); j++) {
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
		for (TSize j = 0; j < i->second.size(); j++) {
			TSize write_tmp[2];
			write_tmp[0] = (TSize) i->second[j].first;
			write_tmp[1] = (TSize) i->second[j].second;
			fwrite(write_tmp, sizeof(TSize), 2, out);
		}
	}
	cout << MSG_DONE << endl << endl;
	cout << this->Lib.size() << " hash notes was exported" << endl;
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
	this->Files.resize((TSize) files_cnt);
	for (TSize i = 0; i < this->Files.size(); i++) {
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

	for (TSize i = 0; i < (TSize) map_size; i++) {
		TSize hash;
		fread(&hash, sizeof(TSize), 1, in);
		TSize vector_cnt;
		fread(&vector_cnt, sizeof(TSize), 1, in);
		for (TSize j = 0; j < (TSize) vector_cnt; j++) {
			TSize dat_in[2];
			fread(dat_in, sizeof(TSize), 2, in);
			this->Lib[(TSize) hash].push_back(make_pair((TSize) dat_in[0], (TSize) dat_in[1]));
		}
	}
	cout << MSG_DONE << endl << endl;
	cout << this->Lib.size() << " hash notes was imported" << endl;
	fclose(in);
	cout << "SUCCESS!" << endl;
}

void MapInit(map <TSize, pair <TSize, map <TSize, TSize>>> *mp, TSize song, TSize time) {
	if (mp->find(song) == mp->end()) {
		(*mp)[song].first = 0;
		(*mp)[song].second[time] = 0;
		return;
	}

	if ((*mp)[song].second.find(time) == (*mp)[song].second.end()) {
		(*mp)[song].second[time] = 0;
		return;
	}
}

bool IsGreater(pair <TSize, TSize> a, pair <TSize, TSize> b) {
	return a.first > b.first;
}
TSize Diff(TSize a, TSize b) {
	if (a > b) {
		return a - b;
	} else {
		return b - a;
	}
}

string TMusicLib::Check(char *filename) {
	printf("Filename: %s\n", filename);
	vector <TSize> hash_arr;
	ReadSingleMP3(filename, &hash_arr, &this->Mh);
	map <TSize, pair <TSize, map <TSize, TSize>>> cnts;
	cout << "Begin searching..." << endl;

	for (TSize i = 0; i < hash_arr.size(); i++) {
		for (TSize j = 0; j < this->Lib[hash_arr[i]].size(); j++) {
			TSize song = this->Lib[hash_arr[i]][j].first;
			TSize time = this->Lib[hash_arr[i]][j].second;
			MapInit(&cnts, song, time);

			cnts[song].second[time - i]++;
			TSize current_value = cnts[song].second[time - i];
			//TSize current_value = ++cnts[song].second[time - i];
			if (current_value > cnts[song].first) {
				cnts[song].first = current_value;
			}

			/*if (cnts.find(this->Lib[hash_arr[i]][j]) != cnts.end()) {

			} else {
				//cnts[this->Lib[hash_arr[i]][j]]
			}*/
		}
	}
	cout << "cnts.size() = " << cnts.size() << endl;
	vector <pair <TSize, TSize>> found(0);
	for (auto i = cnts.begin(); i != cnts.end(); i++) {
		found.push_back(make_pair(i->second.first, i->first));
	}
	cout << MSG_DONE << endl;
	cout << "Found size = " << found.size() << endl;
	if (found.size() < 2) {
		cout << "NOT FOUND: too small same blocks" << endl;
		return NF_MESSAGE;
	}
	sort(found.begin(), found.end(), IsGreater);

	if (Diff(found[0].second, found[1].second) < MIN_DIFF) {
		cout << "NOT FOUND: found same songs" << endl;
		return NF_MESSAGE;
	}


	return this->Files[found[0].second];
}

void TMusicLib::PrintFiles(void) {
	for (TSize i = 0; i < this->Files.size(); i++) {
		cout << this->Files[i] << endl;
	}
}
