/* private.h -
 */

#ifndef _PRIVATE_H_
#define _PRIVATE_H_

#include <clog.h>



#define ASSERT(expr) do {                           \
    if (!(expr)) {                                  \
      CL_ERROR("ASSERTION FAILED: `" #expr "'");    \
    }                                               \
  } while (0)



#endif
