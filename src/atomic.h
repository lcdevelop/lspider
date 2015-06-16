/*************************************************************************
 * Copyright (c) 2015, LiChuang. All rights reserved.
 * Author: lichuang(whlichuang@126.com)
 * Created Time: Fri May 15 00:25:46 2015
 * Description: 
 ************************************************************************/

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

static inline int atomic_add(volatile int *count, int add)
{
    __asm__ __volatile__(
                         "lock xadd %0, (%1);"
                         : "=a"(add)
                         : "r"(count), "a"(add)
                         : "memory"
                        );
    return add;
}

#define __atomic_add64__(mem, add) \
do { \
	asm volatile ( "lock addq %2, %0" \
		: "=m" (*mem) \
		: "m" (*mem), "ir" (add)); \
} while (0)

#define __atomic_sub64__(mem, sub) \
do { \
	asm volatile ( "lock subq %2, %0" \
		: "=m" (*mem) \
		: "m" (*mem), "ir" (sub)); \
} while (0)

static inline int 		// return old value
atomic_swap(volatile void *lockword, int value)
{
    __asm__ __volatile__(
        "lock xchg %0, (%1);"
        : "=a"(value)
        : "r"(lockword), "a"(value)
        : "memory"
        );
    return value;
}

static inline int 		// return old value
atomic_comp_swap(volatile void *lockword, 
                  int exchange,  
                  int comperand)
{
    __asm__ __volatile__(
        "lock cmpxchg %1, (%2)"
        :"=a"(comperand)
        :"d"(exchange), "r"(lockword), "a"(comperand) 
        );
    return comperand;
}

static inline int 		// return old value
atomic_comp_swap64(volatile void *lockword, 
                 int64_t exchange,  
                 int64_t comperand)
{
    __asm__ __volatile__(
        "lock cmpxchg %1, (%2)"
        :"=a"(comperand)
        :"d"(exchange), "r"(lockword), "a"(comperand) 
        );
    return comperand;
}

#define nop()						    __asm__ ("pause" )
#define sfence()					    __asm__ __volatile__ ("lock; addl $0,0(%%esp)": : :"memory")

#define AtomicGetValue(x)			    (atomic_comp_swap(&(x), 0, 0))
#define AtomicSetValue(x, v)		    (atomic_swap(&(x), (v)))
#define AtomicSetValueIf(x, v, ifn)		(atomic_comp_swap(&(x), (v), ifn))

// return new value
#define AtomicDec(c)				    (atomic_add(&(c), -1) - 1)
#define AtomicInc(c)				    (atomic_add(&(c), 1) + 1)

#endif //__ATOMIC_H__
