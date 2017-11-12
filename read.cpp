#include "read.h"
#include "tmpg123.h"

#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <complex>
#include <mpg123.h>

using namespace std;

typedef float TDouble;
typedef complex<TDouble> TComplex;

const size_t BLOCK_SIZE = 4096;
const size_t RANGE_NUM = 4;
const size_t RANGE[RANGE_NUM] = {40, 80, 120, 180};
const size_t RANGE_MIN_VALUE = 40;
const size_t RANGE_MAX_VALUE = 300;
const size_t HASH_MD_CONST = 2;

void ReadFile(vector <TDouble> *arr, mpg123_handle *mh, const char* file) {
	// Open file
	assert(mpg123_open(mh, file) == MPG123_OK);
	// Prevent warning (BEGIN)
	long rate;
	int channels, encoding;
	assert(mpg123_getformat(mh, &rate, &channels, &encoding) == MPG123_OK);
	// Prevent warning (END)
	const int part_size = 1024;
	unsigned char part[part_size];
	size_t bytes_read;
	// Vector for decoded signal
	//vector <TDouble> arr;
	size_t bytes_processed = 0;
	do {
		int err = mpg123_read(mh, part, part_size, &bytes_read);
		arr->resize((bytes_processed + bytes_read) / 4 + 1);
		memcpy((unsigned char*)arr->data() + bytes_processed, part, bytes_read);
		bytes_processed += bytes_read;
		if (err == MPG123_DONE){
			break;
		}
		assert(err == MPG123_OK);
	}while(bytes_read > 0);
	arr->resize(bytes_processed / 4);
	// Close file
	assert(mpg123_close(mh) == MPG123_OK);
	// Process vector with arr
	//return arr;
}

void FFT(vector <TComplex> *arr) {
	size_t size = arr->size();
	if (size <= 1) {
		return;
	}
	vector <TComplex> arr1(size / 2);
	vector <TComplex> arr2(size / 2);

	for (size_t i = 0, j = 0; i < size; i += 2, j++) {
		arr1[j] = (*arr)[i];
		arr2[j] = (*arr)[i + 1];
	}

	FFT(&arr1);
	FFT(&arr2);

	const TDouble coeff = 2 * M_PI / size;

	TComplex w(1.);
	TComplex wn(cos(coeff), sin(coeff));

	for (size_t i = 0; i < size / 2; i++) {
		(*arr)[i] = arr1[i] + w * arr2[i];
		(*arr)[i + size / 2] = arr1[i] - w * arr2[i];
		w *= wn;
	}
}

size_t GetRangeID(size_t freq) {
	size_t res;
	for (res = 0; res < RANGE_NUM; res++) {
		if (RANGE[res] < freq) {
			return res;
		}
	}
	return RANGE_NUM - 1;
}
size_t GetHashBy4Elems(size_t *val) {
	size_t res = val[0] - (val[0] % HASH_MD_CONST);
	res += (val[1] - (val[1] % HASH_MD_CONST)) * 100;
	res += (val[2] - (val[2] % HASH_MD_CONST)) * 100000;
	res += (val[3] - (val[3] % HASH_MD_CONST)) * 100000000;

	return res;
}

void TransformAudioToHashArray(vector <TDouble> *audio, vector <size_t> *out) {
	size_t audio_size = audio->size();
	size_t block_num = audio_size / BLOCK_SIZE;

	//vector <size_t> res(0);
	out->resize(0);

	for (size_t i = 0; i < block_num; i++) {
		vector <TComplex> complex_array(BLOCK_SIZE);
		for (size_t j = 0; j < BLOCK_SIZE; j++) {
			TComplex tmp((*audio)[(i * BLOCK_SIZE) + j], 0);
			complex_array[j] = tmp;
		}

		FFT(&complex_array);

		TDouble highval[RANGE_NUM];
		for (size_t i = 0; i < RANGE_NUM; i++) {
			highval[i] = -1.;
		}
		size_t max_freq[RANGE_NUM];

		for (size_t freq = RANGE_MIN_VALUE; freq < RANGE_MAX_VALUE; freq++) {
			TDouble intensity = log(abs(complex_array[freq]) + 1.);
			size_t id = GetRangeID(freq);

			if (intensity > highval[id]) {
				highval[id] = intensity;
				max_freq[id] = freq;
			}
		}

		out->push_back(GetHashBy4Elems(max_freq));
	}

	//return res;
}

void ReadSingleMP3(char *name, vector <size_t> *out, TMpg123 *mh) {
	vector <TDouble> audio;
	ReadFile(&audio, mh->GetHandle(), name);
	TransformAudioToHashArray(&audio, out);

	/*size_t audio_size = audio.size();
	size_t block_num  audio_size / BLOCK_SIZE;

	vector <vector <TComplex>> */
	// Library deinitialization (END)
}


/*
int main(int argc, char *argv[]) {
	if(argc == 1){
		fprintf(stderr, "USAGE: %s <mp3 file>*\n", argv[0]);
		return 1;
	}
	// Library initialization (BEGIN)
	assert(mpg123_init() == MPG123_OK);
	mpg123_handle *mh = mpg123_new(NULL, NULL);
	assert(mh != NULL);
	long flags = MPG123_MONO_MIX | MPG123_QUIET | MPG123_FORCE_TDouble;
	assert(mpg123_param(mh, MPG123_FLAGS, flags, 0.) == MPG123_OK);
	// Library initialization (END)
	for(int i = 1; i < argc; ++i)
		process_file(mh, argv[i]);
	// Library deinitialization (BEGIN)
	mpg123_delete(mh);
	mpg123_exit();
	// Library deinitialization (END)
}
*/