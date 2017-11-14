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
#include "types.h"

using namespace std;

typedef float TDouble;
typedef complex<TDouble> TComplex;

const TSize BLOCK_SIZE = 4096;
const TSize RANGE_NUM = 4;
const TSize RANGE[RANGE_NUM] = {40, 80, 120, 180};
const TSize RANGE_MIN_VALUE = 40;
const TSize RANGE_MAX_VALUE = 300;
const TSize HASH_MD_CONST = 2;

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
	TSize bytes_read;
	// Vector for decoded signal
	//vector <TDouble> arr;
	TSize bytes_processed = 0;
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
	TSize size = arr->size();
	if (size <= 1) {
		return;
	}
	vector <TComplex> arr1(size / 2);
	vector <TComplex> arr2(size / 2);

	for (TSize i = 0, j = 0; i < size; i += 2, j++) {
		arr1[j] = (*arr)[i];
		arr2[j] = (*arr)[i + 1];
	}

	FFT(&arr1);
	FFT(&arr2);

	const TDouble coeff = 2 * M_PI / size;

	TComplex w(1.);
	TComplex wn(cos(coeff), sin(coeff));

	for (TSize i = 0; i < size / 2; i++) {
		(*arr)[i] = arr1[i] + w * arr2[i];
		(*arr)[i + size / 2] = arr1[i] - w * arr2[i];
		w *= wn;
	}
}

TSize GetRangeID(TSize freq) {
	TSize res;
	for (res = 0; res < RANGE_NUM; res++) {
		if (RANGE[res] < freq) {
			return res;
		}
	}
	return RANGE_NUM - 1;
}
TSize GetHashBy4Elems(TSize *val) {
	TSize res = val[0] - (val[0] % HASH_MD_CONST);
	res += (val[1] - (val[1] % HASH_MD_CONST)) * 100;
	res += (val[2] - (val[2] % HASH_MD_CONST)) * 100000;
	res += (val[3] - (val[3] % HASH_MD_CONST)) * 100000000;

	return res;
}

void TransformAudioToHashArray(vector <TDouble> *audio, vector <TSize> *out) {
	TSize audio_size = audio->size();
	TSize block_num = audio_size / BLOCK_SIZE;

	out->resize(0);

	for (TSize i = 0; i < block_num; i++) {
		vector <TComplex> complex_array(BLOCK_SIZE);
		for (TSize j = 0; j < BLOCK_SIZE; j++) {
			TComplex tmp((*audio)[(i * BLOCK_SIZE) + j], 0);
			complex_array[j] = tmp;
		}

		FFT(&complex_array);

		TDouble highval[RANGE_NUM];
		for (TSize i = 0; i < RANGE_NUM; i++) {
			highval[i] = -1.;
		}
		TSize max_freq[RANGE_NUM];

		for (TSize freq = RANGE_MIN_VALUE; freq < RANGE_MAX_VALUE; freq++) {
			TDouble intensity = log(abs(complex_array[freq]) + 1.);
			TSize id = GetRangeID(freq);

			if (intensity > highval[id]) {
				highval[id] = intensity;
				max_freq[id] = freq;
			}
		}

		out->push_back(GetHashBy4Elems(max_freq));
	}
}

void ReadSingleMP3(char *name, vector <TSize> *out, TMpg123 *mh) {
	vector <TDouble> audio;
	ReadFile(&audio, mh->GetHandle(), name);
	TransformAudioToHashArray(&audio, out);
}
