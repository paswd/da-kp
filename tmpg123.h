#ifndef TMPG123_h
#define TMPG123_h

#include <mpg123.h>

class TMpg123 {
private:
	mpg123_handle *Mh;
	void Create(void);
	void Delete(void);

public:
	TMpg123(void);
	~TMpg123(void);
	void Restart(void);

	mpg123_handle *GetHandle(void);
};

#endif //TMPG123_h
