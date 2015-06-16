#ifndef __LOCKED_QUEUE_H__
#define __LOCKED_QUEUE_H__

#include "mutex_lock.h"



typedef void* ptr;

template <typename TElement, typename TQueue, typename TLock=CMutexLock>
class TLockedQueue
{
public:
	typedef typename TQueue::ENUMERATOR ENUMERATOR;
	TLockedQueue() { Init(); }
	~TLockedQueue() { }

	void Init() {
		m_queue.Init();
	}

	int push_back(TElement* e) {
        int n;
		m_lock.Lock();
		n = m_queue.push_back(e);
		m_lock.Unlock();
        return n;
	}

	int push_front(TElement* e) {
        int n;
		m_lock.Lock();
		n = m_queue.push_front(e);
		m_lock.Unlock();
        return n;
	}

	TElement* pop_front() {
		TElement* e;

		m_lock.Lock();
		e = m_queue.pop_front();
		m_lock.Unlock();
		return e;
	}

	TElement* pop_back() {
		TElement* e;

		m_lock.Lock();
		e = m_queue.pop_back();
		m_lock.Unlock();
		return e;
	}

	TElement* get_back() {
		TElement* e;

		m_lock.Lock();
		e = m_queue.get_back();
		m_lock.Unlock();
		return e;
	}

	TElement* get_front() {
		TElement* e;

		m_lock.Lock();
		e = m_queue.get_front();
		m_lock.Unlock();
		return e;
	}

	void Enumerate(ENUMERATOR f, ptr pData) {
		m_lock.Lock();
		m_queue.Enumerate(f, pData);
		m_lock.Unlock();
	}

	int remove(TElement* p) {
        int n;
		m_lock.Lock();
		n = m_queue.remove(p);
		m_lock.Unlock();
        return n;
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
};

#endif
