#ifndef NAMED_MUTEX
#define NAMED_MUTEX

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

class NamedMutex {
public: // c-tor & d-tor
	NamedMutex(const char *name);
	~NamedMutex() {}
public: // Member funcs
	void lock(void);
	bool lock(int time_out);
	void unlock(void);
private: // Data memebers
	int _sem_id;
	key_t _key;
	int _locked;
};

#endif
