/***************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Sat May 16 22:24:08 2015
 * Description: 
 **************************************************************************/
 
/**

 #include "lthread.h"
 using namespace lthread;

 class MyThread : public LThread
 {
    public:
    void run()
     {
         printf("the thread [%lu] is running.\n", current_thread());
         sleep(1);
     }
 };

 int main(int argc, char* argv[])
 {
     MyThread t1;
     MyThread t2;
     t1.start();
     t2.start();
     t1.wait();
     t2.wait();
     printf("main thread [%lu] exit\n", pthread_self());
     return 0;
 }

 *  
 **/

#ifndef  BUILD_SEA_LTHREAD_H
#define  BUILD_SEA_LTHREAD_H


#include <iostream>
//using namespace std;
#include <sys/time.h>
#include <pthread.h>
#include <stdint.h>

namespace lthread
{

/**
 * 自动锁类
 */
struct auto_lock_t
{
    auto_lock_t(pthread_mutex_t* mutex) : m_mutex(mutex)
    {
        pthread_mutex_lock(m_mutex);
    }
    ~auto_lock_t()
    {
        pthread_mutex_unlock(m_mutex);
    }
    pthread_mutex_t* m_mutex;
};

/**
 * 线程执行的真正实例类，封装了线程的实现细节，可以实现不同平台的不同实现
 */
class LThreadInstance
{
public:

    /// 参数数组，分别存储LThread指针和LThreadInstance指针
    void *args[2];
    /// 线程执行是否完成
    bool finished : 1;
    /// 线程是否正在执行
    bool running  : 1;
    /// 线程执行完成condition信号
    pthread_cond_t thread_done;
    /// 线程id
    pthread_t thread_id;
    /// 互斥锁
    pthread_mutex_t mutex;

    /**
     * @brief 初始化成员变量
     * @return  void 
     * @author lichuang
     * @date 2012/06/03 12:41:00
    **/
    void init();
    /**
     * @brief 销毁成员变量
     * @return  void 
     * @author lichuang
     * @date 2012/06/03 12:41:18
    **/
    void deinit();
    /**
     * @brief 终止线程
     * @return  void 
     * @author lichuang
     * @date 2012/06/03 12:41:40
    **/
    void terminate();

    /**
     * @brief 启动静态函数，启动的线程中调用
     * @param [in/out] _arg   : void*
     * @return  void* 
     * @author lichuang
     * @date 2012/06/03 12:41:54
    **/
    static void *start(void *_arg);
    /**
     * @brief 线程执行结束后执行，唤醒等待信号
     * @return  void 
     * @author lichuang
     * @date 2012/06/03 12:42:30
    **/
    static void finish(void *);
    /**
     * @brief 获取当前所在线程的LThreadInstance指针
     * @return  LThreadInstance* 
     * @author lichuang
     * @date 2012/06/03 12:43:01
    **/
    static LThreadInstance* current();
};

/**
 * 线程类基类，需要继承并实现run()方法
 */
class LThread
{
public:
    /**
     * @brief 构造函数
     * @author lichuang
     * @date 2012/06/03 12:43:50
    **/
    LThread();
    /**
     * @brief 析构函数
     * @author lichuang
     * @date 2012/06/03 12:43:59
    **/
    virtual ~LThread();
    /**
     * @brief 启动线程
     * @return  void 
     * @author lichuang
     * @date 2012/06/02 14:12:47
    **/
    int start();
    /**
     * @brief 终止线程，非逼不得已不要用，因操作系统调度原因，可能不会立即终止线程，
     * 因此调用后最好接着调用bool wait(unsigned long time)来确定线程结束，否则后果自负
     * @return  void 
     * @author lichuang
     * @date 2012/06/02 14:13:04
    **/
    void terminate();
    /**
     * @brief 返回当前线程号
     * @return  pthread_t 
     * @author lichuang
     * @date 2012/06/03 10:48:36
    **/
    pthread_t current_thread();
    /**
     * @brief 等待线程执行结束，单位毫秒，默认为一直等待到结束
     * @param [in/out] time   : unsigned long
     * @return  bool 
     * @author lichuang
     * @date 2012/06/03 10:54:15
    **/
    bool wait(unsigned long time = (unsigned long)-1);
    /**
     * @brief 判断线程是否执行完成
     * @return  bool 
     * @author lichuang
     * @date 2012/06/06 10:01:18
    **/
    bool finished() const;
    /**
     * @brief 判断线程是否正在运行
     * @return  bool 
     * @author lichuang
     * @date 2012/06/06 10:01:34
    **/
    bool running() const;
    /**
     * @brief 初始化main_instance静态变量
     * @return  void 
     * @author lichuang
     * @date 2012/06/03 12:45:42
    **/
    static void initialize();
    /**
     * @brief Ends the execution of the calling thread and wakes up any threads
     * waiting for its termination.
     * @return  void 
     * @author lichuang
     * @date 2012/06/03 10:47:31
    **/
    static void exit();
protected:
    /**
     * @brief 线程执行代码，由用户实现
     * @return  void 
     * @author lichuang
     * @date 2012/06/02 14:13:22
    **/
    virtual void run() = 0;

private:
    /// 线程类的实际实现类指针
    LThreadInstance * d;
    friend class LThreadInstance;
};

}// namespace lthread

#endif  //BUILD_SEA_LTHREAD_H

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
