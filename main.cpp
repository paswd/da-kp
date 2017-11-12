#include <iostream>
#include <vector>
#include <limits>

#include "read.h"
#include "tmpg123.h"

using namespace std;

int main(int argc, char *argv[]) {
	TMpg123 mh;
	char name[32] = "test.mp3\0";
	//vector <float> mus = ReadSingleMP3(name);
	vector <size_t> mus;
	ReadSingleMP3(name, &mus, &mh);
	for (size_t i = 0; i < mus.size(); i++) {
		cout << mus[i] << endl;
	}

	return 0;
}
