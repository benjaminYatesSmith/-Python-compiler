#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <inttypes.h>
#include <sys/types.h>

#include <errno.h>

#include <unitest/logging.h>
#include <unitest/clock.h>


struct timespec time_point;

double  elapsed_nsec( void ) {
  size_t           ns;
  struct timespec now;

  clock_gettime( CLOCK_MONOTONIC, &now );

  ns  = 1e9 * ( now.tv_sec - time_point.tv_sec );
  ns += now.tv_nsec - time_point.tv_nsec;

  time_point = now;

  return ns;
}

double  elapsed_usec( void ) {
  return elapsed_nsec() / 1e3;
}

double  elapsed_msec( void ) {
  return elapsed_nsec() / 1e6;
}
