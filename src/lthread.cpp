/***************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sat May 16 23:18:47 2015
 * Description: 
 **************************************************************************/

#include "lthread.h"

using lthread::LThreadInstance;
using lthread::LThread;
using lthread::auto_lock_t;

/// 设置pthread_key只需执行一次的锁
static pthread_once_t storage_key_once = PTHREAD_ONCE_INIT;

/// pthread_key用来存储LThreadInstance指针
static pthread_key_t storage_key;

/// 创建pthread_key
static void create_storage_key()
{
    pthread_key_create( &storage_key, NULL );
}

// 静态变量，用来存储LThreadInstance实例
static LThreadInstance main_instance = 
{
    { 0, &main_instance }, 0, 1, PTHREAD_COND_INITIALIZER, 0, PTHREAD_MUTEX_INITIALIZER
};

////////  LThreadInstance  /////////////////

void LThreadInstance::init()
{
    args[0] = args[1] = 0;
    finished = false;
    running = false;
    pthread_cond_init(&thread_done, NULL);
    pthread_mutex_init(&mutex, NULL);
    thread_id = 0;
    LThread::initialize();
}

void LThreadInstance::deinit()
{
    pthread_cond_destroy(&thread_done);
    pthread_mutex_destroy(&mutex);
}

void LThreadInstance::terminate()
{
    if (!thread_id) 
    {
        return;
    }
    pthread_cancel(thread_id);
}

void *LThreadInstance::start(void *_arg)
{
    void **arg = (void **) _arg;
    pthread_once( &storage_key_once, create_storage_key );
    pthread_setspecific( storage_key, arg[1] );
    pthread_cleanup_push( LThreadInstance::finish, arg[1] );
    pthread_testcancel();

    ((LThread *)arg[0])->run();

    pthread_cleanup_pop(true);
    return 0;
}

void LThreadInstance::finish(void *)
{
    LThreadInstance *d = current();
    if (!d)
    {
        std::cerr << "LThread: internal error: zero data for running thread." << std::endl;
        return;
    }
    auto_lock_t(&d->mutex);
    d->args[0] = d->args[1] = 0;
    d->finished = true;
    d->running = false;
    d->thread_id = 0;
    pthread_cond_broadcast(&d->thread_done);
}

LThreadInstance* LThreadInstance::current()
{
    pthread_once( &storage_key_once, create_storage_key );
    LThreadInstance *ret = (LThreadInstance *) pthread_getspecific( storage_key );
    return ret;
}

/////////////////  LThread  ////////////////

LThread::LThread()
{
    d = new LThreadInstance;
    d->init();
}
LThread::~LThread()
{
    delete d;
}

int LThread::start()
{
    auto_lock_t(&d->mutex);
    if (d->running)
    {
        pthread_cond_wait(&d->thread_done, &d->mutex);
    }
    d->running = true;
    d->finished = false;
    d->args[0] = this;
    d->args[1] = d;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&d->thread_id, NULL, LThreadInstance::start, d->args);
    pthread_attr_destroy( &attr );
    return ret;
}

void LThread::terminate()
{
    auto_lock_t(&d->mutex);
    if (d->finished || !d->running)
    {
        return;
    }
    d->terminate();
}

pthread_t LThread::current_thread()
{
    return pthread_self();
}

bool LThread::wait(unsigned long time)
{
    // 这里就奇怪了，用auto_lock_t之后锁不会释放
    pthread_mutex_lock(&d->mutex);
    if (d->thread_id == pthread_self())
    {
        std::cerr << "LThread::wait: thread tried to wait on itself" << std::endl;
        pthread_mutex_unlock(&d->mutex);
        return false;
    }
    if (d->finished || ! d->running)
    {
        pthread_mutex_unlock(&d->mutex);
        return true;
    }
    int ret;
    if (time != (unsigned long)-1) 
    {
        struct timeval tv;
        gettimeofday(&tv, 0);

        timespec ti;
        ti.tv_nsec = (tv.tv_usec + (time % 1000) * 1000) * 1000;
        ti.tv_sec = tv.tv_sec + (time / 1000) + (ti.tv_nsec / 1000000000);
        ti.tv_nsec %= 1000000000;
        ret = pthread_cond_timedwait(&d->thread_done, &d->mutex, &ti);
    } 
    else
    {
        ret = pthread_cond_wait(&d->thread_done, &d->mutex);
    }
    pthread_mutex_unlock(&d->mutex);
    return (ret == 0);
}

void LThread::initialize()
{
    pthread_once(&storage_key_once, create_storage_key);
    pthread_setspecific(storage_key, &main_instance);
}

void LThread::exit()
{
    pthread_exit(0);
}

bool LThread::finished() const
{
    auto_lock_t(&d->mutex);
    return d->finished;
}

bool LThread::running() const
{
    auto_lock_t(&d->mutex);
    return d->running;
}

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
