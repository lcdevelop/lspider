#ifndef __LINK_H__
#define __LINK_H__

/*
    SLINK - single link
*/
typedef struct _SLINK{
   _SLINK* _next;
} SLINK, *PSLINK;

//initialize the link 
#define SLINK_INITIALIZE(_head)           ((_head)->_next = NULL)

//check if list is empty
#define SLINK_IS_EMPTY(_head)             ((_head)->_next == NULL)

//pop the first item in list  
#define SLINK_POP(_head)                  (_head)->_next;\
                                          (_head)->_next =  (_head)->_next->_next;
//push an item to the head of list    
#define SLINK_PUSH(_head,_link)           (_link)->_next =  (_head)->_next;\
                                          (_head)->_next =  (_link)

/*
    DLINK - double link
*/
typedef struct _DLINK{
   _DLINK* _prev;
   _DLINK* _next;
} DLINK, *PDLINK;

//initialize the link 
#define DLINK_INITIALIZE(_head)            ((_head)->_next = (_head)->_prev = (_head))

//check if list is empty
#define DLINK_IS_EMPTY(_head)              ((_head)->_next == (_head))

//insert item after the _head item
#define DLINK_INSERT_NEXT(_head,_dlink)    (_dlink)->_next = (_head)->_next;		\
                                           (_dlink)->_prev = (_head);				\
                                           (_head)->_next->_prev = (_dlink);		\
                                           (_head)->_next = (_dlink)

//insert item previous to the _head item         
#define DLINK_INSERT_PREV(_head,_dlink)    (_dlink)->_prev = (_head)->_prev;		\
                                           (_dlink)->_next = (_head);				\
                                           (_head)->_prev->_next = (_dlink);		\
                                           (_head)->_prev = (_dlink)
//remove the item from list
#define DLINK_REMOVE(_dlink)               (_dlink)->_prev->_next = (_dlink)->_next;\
                                           (_dlink)->_next->_prev = (_dlink)->_prev
//extract the item previous to _head
#define DLINK_EXTRUCT_PREV(_head)          (_head)->_prev;							\
                                           DLINK_REMOVE((_head)->_prev)
//extract the item after the _head item
#define DLINK_EXTRUCT_NEXT(_head)           (_head)->_next;							\
                                            DLINK_REMOVE((_head)->_next)

// replace node _old with node _new in the list
#define DLINK_REPLACE(_old, _new)           (_old)->_next->_prev = (_new);	\
                                            (_old)->_prev->_next = (_new);	\
                                            (_new)->_next = (_old)->_next;	\
                                            (_new)->_prev = (_old)->_prev

#ifdef _DEBUG
#if defined(__x86_64__)
#define INVALID_POINTER                     (DLINK*)0xCCCCCCCCCCCCCCCCULL
#else
#define INVALID_POINTER                     (DLINK*)0xCCCCCCCCUL
#endif

#define DLINK_IS_STANDALONE(_dlink)         (((_dlink)==(_dlink)->_next                 \
                                                && (_dlink)==(_dlink)->_prev)           \
                                            || (NULL==(_dlink)->_next                   \
                                                && NULL==(_dlink)->_prev)               \
                                            || (INVALID_POINTER==(_dlink)->_next        \
                                                && INVALID_POINTER==(_dlink)->_prev)    \
                                            )
#define PTR_IS_NULL(_ptr)                   (NULL==(_ptr) || INVALID_POINTER==(DLINK*)(_ptr))

#else
#define DLINK_IS_STANDALONE(_dlink)         (((_dlink)==(_dlink)->_next                 \
                                                && (_dlink)==(_dlink)->_prev)           \
                                            || (NULL==(_dlink)->_next                   \
                                                && NULL==(_dlink)->_prev)               \
                                            )
#define PTR_IS_NULL(_ptr)                   (NULL==(_ptr))

#endif

//get the object of type "type" that contains the field "field" stating in address "address"
#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) ((type *)( 				            \
                                                 (char*)(address) - 			    \
                                                 (long)(&((type *)0)->field)))
#endif

#endif
