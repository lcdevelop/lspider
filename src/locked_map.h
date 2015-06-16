#include <map>
#include <pthread.h>

template <typename KEY_TYPE, typename VALUE_TYPE>
class LockedMap : public std::map<KEY_TYPE, VALUE_TYPE>
{
public:
    LockedMap() {
        pthread_mutex_init(&_mutex, NULL);
    }

    ~LockedMap() {
        pthread_mutex_destroy(&_mutex);
    }

    bool set(KEY_TYPE key, VALUE_TYPE & value) {
        pthread_mutex_lock(&_mutex);
        bool ret = insert(std::pair<KEY_TYPE, VALUE_TYPE>(key, value));
        pthread_mutex_unlock(&_mutex);
        return ret;
    }

    VALUE_TYPE* get(KEY_TYPE key) {
        VALUE_TYPE *ret = NULL;
        pthread_mutex_lock(&_mutex);
        typename std::map<KEY_TYPE,VALUE_TYPE>::iterator iter;
        if (std::end != (iter = find(key))) {
            ret = &iter->second;
        }
        pthread_mutex_unlock(&_mutex);
        return ret;
    }

private:
    pthread_mutex_t _mutex;
};
