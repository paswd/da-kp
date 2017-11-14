#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include <fstream>

#include "read.h"
#include "musiclib.h"
#include "types.h"

using namespace std;

const TSize MIN_ARGS_NUM_INDEX = 6;
const TSize MIN_ARGS_NUM_SEARCH = 8;

const string MSG_FEW_ARGS = "Too few arguments";
const string MSG_ERR_PARAM = "Incorrect paramerer";
const string MSG_INDEX_SUCCESS = "Index job has been successfully completed";

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cout << MSG_FEW_ARGS << endl;
		return 0;
	}
	string type(argv[1]);

	TMusicLib lib;

	if (type == "index") {
		cout << "=====" << endl;
		cout << "INDEX" << endl;
		cout << "=====" << endl;
		if ((TSize) argc < MIN_ARGS_NUM_INDEX) {
			cout << MSG_FEW_ARGS << endl;
			return 0;
		}
		TSize input_id = 0;
		TSize output_id = 0;
		bool is_input = false;
		bool is_output = false;
		for (TSize i = 2; i < (TSize) argc - 1; i += 2) {
			string action(argv[i]);
			if (action == "--input") {
				is_input = true;
				input_id = i + 1;
				continue;
			}
			if (action == "--output") {
				is_output = true;
				output_id = i + 1;
				continue;
			}
		}

		if (is_input && is_output) {
			lib.Build(argv[input_id]);
			cout << endl;
			lib.Export(argv[output_id]);

			cout << MSG_INDEX_SUCCESS << endl;
		} else {
			cout << MSG_FEW_ARGS << endl;
		}
		return 0;
	}
	if (type == "search") {
		cout << "======" << endl;
		cout << "SEARCH" << endl;
		cout << "======" << endl;
		if ((TSize) argc < MIN_ARGS_NUM_SEARCH) {
			cout << MSG_FEW_ARGS << endl;
			return 0;
		}
		TSize input_id = 0;
		TSize index_id = 0;
		TSize output_id = 0;
		bool is_input = false;
		bool is_index = false;
		bool is_output = false;
		for (TSize i = 2; i < (TSize) argc - 1; i += 2) {
			string action(argv[i]);
			if (action == "--input") {
				is_input = true;
				input_id = i + 1;
				continue;
			}
			if (action == "--index") {
				is_index = true;
				index_id = i + 1;
				continue;
			}
			if (action == "--output") {
				is_output = true;
				output_id = i + 1;
				continue;
			}
		}

		if (is_input && is_output && is_index) {
			lib.Import(argv[index_id]);
			cout << endl;
			string res = lib.Check(argv[input_id]);
			ofstream ofs(argv[output_id], std::ofstream::out);
			ofs << res << endl;
			ofs.close();
		} else {
			cout << MSG_FEW_ARGS << endl;
		}
		return 0;
	}

	cout << MSG_ERR_PARAM << endl;
	
	return 0;
}
