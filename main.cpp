#include <iostream>
#include <vector>
#include <limits>

#include "read.h"
#include "musiclib.h"

using namespace std;

int main(int argc, char *argv[]) {
	TMusicLib lib;
	char name[32] = "audiolist.txt\0";
	char ex[32] = "lib.bin\0";
	char ex1[32] = "libn.bin\0";
	lib.Build(name);
	lib.Export(ex);
	lib.Import(ex);
	lib.PrintFiles();
	lib.Export(ex1);
	/*TMpg123 mh;
	char name[32] = "test.mp3\0";
	//vector <float> mus = ReadSingleMP3(name);
	vector <size_t> mus;
	ReadSingleMP3(name, &mus, &mh);
	for (size_t i = 0; i < mus.size(); i++) {
		cout << mus[i] << endl;
	}*/


	return 0;
}
