/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Fri 29 May 2015 04:48:53 PM CST
 * Description: 
 ************************************************************************/

#ifndef __KEYED_QUEUE_H__
#define __KEYED_QUEUE_H__

#include <map>
#include "DoubleList.h"

using std::map;
using std::pair;
using std::less;

/*
 *	TElement must contain the following fields:
 *		DLINK link;
 *      TKey key;
 */
template<typename Key, typename TElement, typename Traits=less<Key> >
class TKeyedQueue
{
protected:
	typedef map<Key, TElement*, Traits> Indexer;
	typedef TLinkedList<TElement> Queue;
    typedef typename Indexer::value_type value_type;
public:
	typedef typename Indexer::iterator Iterator;
	typedef bool (*ENUMERATOR)(TElement* pE, ptr pData);	// return false to stop the enumeration

	TKeyedQueue() {}
	~TKeyedQueue() {}

	bool push_back(TElement* pElement, TElement** ppOldElement=NULL) {
        ASSERT(NULL != pElement);
        ASSERT(DLINK_IS_STANDALONE(&pElement->link));

		pair<Iterator, bool> pr;
		pr = m_Indexer.insert(value_type(pElement->key, pElement));
		if (pr.second) {
			m_Queue.push_back(pElement);
			return true;
		}
        if (ppOldElement) {
			*ppOldElement = pr.first->second;
        }
		return false;
	}

    TElement* push_back_and_replace(TElement* pNew) {
        ASSERT(NULL != pNew);
        ASSERT(DLINK_IS_STANDALONE(&pNew->link));

        pair<Iterator, bool> pr;
        pr = m_Indexer.insert(value_type(pNew->key, pNew));
        if (pr.second) {
            m_Queue.push_back(pNew);
        }
        else {
            TElement* pOld = pr.first->second;
            if (pOld != pNew) {                     // prevent double insertion
                pr.first->second = pNew;
                DLINK_REPLACE(&pOld->link, &pNew->link);
                DLINK_INITIALIZE(&pOld->link);
                return pOld;
            }
        }
        return NULL;
    }

	TElement* pop_front() {
		TElement* pE = m_Queue.pop_front();
		if (pE) {
			Iterator it = m_Indexer.find(pE->key);
			ASSERT(it != m_Indexer.end());
			m_Indexer.erase(it);
		}
		return pE;
	}

    TElement* front_to_back() {
        TElement* pE = m_Queue.pop_front();
        if (pE) {
            m_Queue.push_back(pE);
        }
        return pE;
    }

    bool move_to_back(TElement* pElement) {
        ASSERT(NULL != pElement);
        Iterator it = m_Indexer.find(pElement->key);
        if (m_Indexer.end()!=it && pElement==it->second) {
            m_Queue.remove(pElement);
            m_Queue.push_back(pElement);
            return true;
        }
        return false;
    }

    bool move_to_front(TElement* pElement) {
        ASSERT(NULL != pElement);
        Iterator it = m_Indexer.find(pElement->key);
        if (m_Indexer.end()!=it && pElement==it->second) {
            m_Queue.remove(pElement);
            m_Queue.push_front(pElement);
            return true;
        }
        return false;
    }

	TElement* get_front() {
		return m_Queue.get_front();
	}

	TElement* find(Key k) {
		Iterator it = m_Indexer.find(k);
		if (it != m_Indexer.end()) {
			return it->second;
		}
		return NULL;
	}

	TElement* remove(Key k) {
		Iterator it = m_Indexer.find(k);
		if (it != m_Indexer.end()) {
			TElement* pE = it->second;
			m_Queue.remove(pE);
			m_Indexer.erase(it);
			return pE;
		}
		return NULL;
	}

	bool remove(TElement* pElement) {
        ASSERT(NULL != pElement);
		Iterator it = m_Indexer.find(pElement->key);
		if (m_Indexer.end()!=it && pElement==it->second) {
			m_Indexer.erase(it);
			m_Queue.remove(pElement);
			return true;
		}
		return false;
	}

	bool Enumerate(ENUMERATOR f, ptr pData) {
		return m_Queue.Enumerate(f, pData);
	}

	int size() const { return m_Queue.size(); }

protected:
	Indexer m_Indexer;          // indexer
	Queue m_Queue;              // the queue
};

#endif //__KEYED_QUEUE_H__
