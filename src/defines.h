#include <stdlib.h>

#define ASSERT(c)   if (!(c)) {															\
    abort(); \
}
#define ASSERT_EQUAL(x, y)  if ((x) != (y)) {                                           \
    abort(); \
}
