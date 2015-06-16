#ifndef __MUTEX_LOCK_H__
#define __MUTEX_LOCK_H__


#include <pthread.h>

class CMutexLock
{
public:
	CMutexLock() {
        pthread_mutex_init(&_mutex, NULL);
    }

	~CMutexLock() {
        pthread_mutex_destroy(&_mutex);
    }

    void Init() {
    }

	void Lock() {
        pthread_mutex_lock(&_mutex);
    }

	bool Unlock()
    {
        pthread_mutex_unlock(&_mutex);
        return true;
    }

private:
    pthread_mutex_t _mutex;
};

#endif
