#ifndef _SYNCED_Q_H_
#define _SYNCED_Q_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <assert.h>
#include "mutex_lock.h"

template <typename TElement, typename TQueue, typename TLock=CMutexLock>
class TSyncedQueue
{
public:
	TSyncedQueue() { Init(); }
	~TSyncedQueue() { sem_destroy(&_sem); }
	
	void Init() {
		m_queue.Init();
		m_lock.Init();
        if (0 != sem_init(&_sem, 0, (unsigned int)0)) {
            perror("sem_init fail\n");
            abort();
        }
	}
	
	void push_back(TElement* e) {
		m_lock.Lock();
		m_queue.push_back(e);
		m_lock.Unlock();
        sem_post(&_sem);
	}
	
	void push_front(TElement* e) {
		m_lock.Lock();
		m_queue.push_front(e);
		m_lock.Unlock();
        sem_post(&_sem);
	}
	
	TElement* pop_front() {
		TElement* e;
        sem_wait(&_sem);
		m_lock.Lock();
		e = m_queue.pop_front();
		m_lock.Unlock();
		assert(NULL != e);
		return e;
	}
	
	TElement* pop_back() {
		TElement* e;
        sem_wait(&_sem);
		m_lock.Lock();
		e = m_queue.pop_back();
		m_lock.Unlock();
		
		assert(NULL != e);
		return e;
	}
	
	int size() {
		int n;
		m_lock.Lock();
		n = m_queue.size();
		m_lock.Unlock();
		return n;
	}

private:
	TQueue m_queue;
	TLock m_lock;
    sem_t _sem;
};

#endif
