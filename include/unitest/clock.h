
#ifndef _CLOCK_H_
#define _CLOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

  extern struct timespec time_point;

  double  elapsed_nsec( void );
  double  elapsed_usec( void );
  double  elapsed_msec( void );

#ifdef __cplusplus
}
#endif

#endif /* _CLOCK_H_ */
