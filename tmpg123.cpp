#include "tmpg123.h"

#include <iostream>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <mpg123.h>

TMpg123::TMpg123(void) {
	// Library initialization (BEGIN)
	assert(mpg123_init() == MPG123_OK);
	this->Mh = mpg123_new(NULL, NULL);
	assert(this->Mh != NULL);
	long flags = MPG123_MONO_MIX | MPG123_QUIET | MPG123_FORCE_FLOAT;
	assert(mpg123_param(this->Mh, MPG123_FLAGS, flags, 0.) == MPG123_OK);
	// Library initialization (END)
}

TMpg123::~TMpg123(void) {
	// Library deinitialization (BEGIN)
	mpg123_delete(this->Mh);
	mpg123_exit();
}

mpg123_handle *TMpg123::GetHandle(void) {
	return this->Mh;
}
