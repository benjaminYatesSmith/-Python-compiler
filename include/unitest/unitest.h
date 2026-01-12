/**
 * @file unitest.h
 * @author François Cayre <cayre@uvolante.org>
 * @brief Unit testing.
 *
 * Unit testing.
 *
 * Kudos to Nicolas "Evil Tester" Castagné.
 *
 */

#ifndef _UNITEST_H_
#define _UNITEST_H_

#ifdef __cplusplus
extern "C" {
#endif

  /*
     Use https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#256-colors
     to implement your very own distinctive bad taste here:
  */
#define TEST_STYLE_SUCCESS()      STYLE( this.stderr_orig, 118, ST_BOLD )
#define TEST_STYLE_INFO()         STYLE( this.stderr_orig, 248, ST_BOLD )
#define TEST_STYLE_ERROR()        STYLE( this.stderr_orig, 196, ST_BOLD )
#define TEST_STYLE_EMPH()         STYLE( this.stderr_orig, 160, ST_BOLD )
#define TEST_STYLE_EMPH_LIGHT()   STYLE( this.stderr_orig, 105, ST_UNDERLINE";"ST_ITALIC";"ST_FAINT )
#define TEST_STYLE_DESCR()        STYLE( this.stderr_orig, 173, ST_BOLD";"ST_ITALIC )
#define TEST_STYLE_SKIPPED()      STYLE( this.stderr_orig, 208, ST_BOLD )

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <setjmp.h>
#include <time.h>

#include <unitest/logging.h>

#ifdef VERBOSE
#undef VERBOSE
#define VERBOSE( fmt, ... ) if ( this.env.verbose )             \
    MSG_FMT( 0, "VERBOSE", 34, ST_FAINT, fmt, ##__VA_ARGS__ )
#endif

  typedef struct {
    unsigned    total;
    unsigned    passed;
    unsigned    failed;
    unsigned    interrupted;
    unsigned    aborted;
    unsigned    segfaulted;
    unsigned    untested;
    unsigned    timed_out;
  } test_counter_t;

  typedef struct {

    struct timespec    time_point;

    struct {
      struct sigaction orig_SIGSEGV;
      struct sigaction orig_SIGFPE;
      struct sigaction orig_SIGALRM;
      struct sigaction orig_SIGINT;
      struct sigaction orig_SIGILL;
      struct sigaction orig_SIGTERM;
      struct sigaction orig_SIGABRT;
      sigjmp_buf   restart;
      char        *TEST_ORACLES;
      int          TEST_TIMEOUT;
      char         real_prog_name[ 1024 ];
      char        *prog_name;
      char        *test_name;
      int          verbose;
      int          debug;
      int          expect_abort;
      char        *func;
      int          line;
      int          oracle_line;
    }            env;

    test_counter_t suite;
    test_counter_t total;

    struct {
      struct {
        int           unlink;
        char          file[1024];
      }             captured;
    }             orc;

    FILE *stdout_orig;
    FILE *stderr_orig;

    int   cont_on_sigsegv;
    int   in_oracle_test;
    char *captured_output;

  } test_t;

  extern test_t this;

  void    unit_test( int argc, char *argv[] );
  void    posix_signal_handler( int sig, siginfo_t *siginfo, void *context );
  void    set_signal_handler( void );
  void    reset_signal_handler( void );

#define check_assert( test ) ret__ = !!(test)
#define check_abort( test )  (void)(test)

#define test_driver( exp_abort, do_check, test, comment, ... ) do {     \
    int      ret__ = 0;                                                 \
    unsigned saved_ntests_aborted = this.suite.aborted;                 \
    if ( this.in_oracle_test ) {                                        \
      FATAL( "[%s:%d] DO NOT MIX test_{assert,abort} inside test_oracle()!\n", \
             this.env.func, this.env.line );                            \
    }                                                                   \
    this.env.func = (char*)__func__;                                    \
    this.env.line = __LINE__;                                           \
    set_signal_handler();                                               \
    this.env.expect_abort         = exp_abort;                          \
    this.suite.total++;                                                 \
    this.total.total++;                                                 \
    if ( this.env.verbose ) {                                           \
      TEST_STYLE_DESCR();                                               \
      fprintf( this.stderr_orig, comment, ## __VA_ARGS__ );             \
      STYLE_RESET( this.stderr_orig );                                  \
      fprintf( this.stderr_orig, ": " );                                \
    }                                                                   \
    if ( this.env.debug && exp_abort ) {                                \
      if ( this.env.verbose ) {                                         \
        TEST_STYLE_SKIPPED();                                           \
        fprintf( this.stderr_orig, "NOT TESTABLE IN DEBUGGER (shared SIGABRT)" ); \
        STYLE_RESET( this.stderr_orig );                                \
        fprintf( this.stderr_orig, ".\n" );                             \
      }                                                                 \
      this.suite.total--;                                               \
      this.total.total--;                                               \
      this.total.untested++;                                            \
      break;                                                            \
    }                                                                   \
    if ( 0 == sigsetjmp( this.env.restart, 1 ) ) {                      \
      alarm( this.env.TEST_TIMEOUT );                                   \
      elapsed_nsec();                                                   \
      do_check( test );                                                 \
    }                                                                   \
    else {                                                              \
      ret__ = this.env.expect_abort ?                                   \
        ( this.suite.aborted == 1 + saved_ntests_aborted ) : 0;         \
    }                                                                   \
    alarm( 0 );                                                         \
    reset_signal_handler();                                             \
    if ( ret__ ) {                                                      \
      this.suite.passed++;                                              \
      this.total.passed++;                                              \
      if ( exp_abort ) this.suite.aborted--;                            \
      if ( this.env.verbose ) {                                         \
        TEST_STYLE_SUCCESS();                                           \
        fprintf( this.stderr_orig, "PASSED" );                          \
        STYLE_RESET( this.stderr_orig );                                \
      }                                                                 \
    }                                                                   \
    else {                                                              \
      if ( this.env.verbose ) {                                         \
        TEST_STYLE_ERROR();                                             \
        fprintf( this.stderr_orig, "FAILED" );                          \
        STYLE_RESET( this.stderr_orig );                                \
        if ( !ret__ ) {                                                 \
          if ( this.suite.aborted == saved_ntests_aborted ) {           \
            fprintf( this.stderr_orig, " %sion ",                       \
                     exp_abort ? "abort" : "assert" );                  \
            TEST_STYLE_ERROR();                                         \
            fprintf( this.stderr_orig, "%s", #test );                   \
            STYLE_RESET( this.stderr_orig );                            \
          }                                                             \
          else {                                                        \
            fprintf( this.stderr_orig, " (aborted)" );                  \
          }                                                             \
        }                                                               \
        fprintf( this.stderr_orig, " at " );                            \
        TEST_STYLE_EMPH_LIGHT();                                        \
        fprintf( this.stderr_orig, "%s:%s:%d", this.env.prog_name,      \
                 __func__, __LINE__ );                                  \
        STYLE_RESET( this.stderr_orig );                                \
      }                                                                 \
    }                                                                   \
    if ( this.env.verbose ) {                                           \
      fprintf( this.stderr_orig, ".\n" );                               \
    }                                                                   \
  } while ( 0 )

#define test_assert( test, ... ) test_driver( 0, check_assert, test, ## __VA_ARGS__ )
#define test_abort( test, ... )  test_driver( 1, check_abort , test, ## __VA_ARGS__ )

#define test_suite__( msg, ... ) do {                                   \
    if ( this.env.test_name ) {                                         \
      if ( this.suite.passed != this.suite.total ) {                    \
        TEST_STYLE_ERROR();                                             \
        fprintf( this.stderr_orig, "FAILED " );                         \
        TEST_STYLE_EMPH();                                              \
        fprintf( this.stderr_orig, "%u test%s",                         \
                 this.suite.total - this.suite.passed,                  \
                 this.suite.total - this.suite.passed > 1 ? "s" : "");	\
        STYLE_RESET( this.stderr_orig );                                \
        fprintf( this.stderr_orig, " (out of %u)", this.suite.total );  \
        if ( this.suite.interrupted || this.suite.timed_out ||          \
             this.suite.aborted || this.suite.segfaulted ) {            \
          int n = this.suite.segfaulted;                                \
          fprintf( this.stderr_orig, ", of which " );                   \
          if ( this.suite.segfaulted ) {                                \
            TEST_STYLE_EMPH();                                          \
            STYLE( this.stderr_orig, 196, ST_BLINK );                   \
            fprintf( this.stderr_orig, "%u segfaulted", this.suite.segfaulted ); \
            STYLE_RESET( this.stderr_orig );                            \
          }                                                             \
          if ( this.suite.aborted ) {                                   \
            if ( n ) fprintf( this.stderr_orig, ", " );                 \
            n += this.suite.aborted;                                    \
            TEST_STYLE_EMPH();                                          \
            fprintf( this.stderr_orig, "%u aborted", this.suite.aborted ); \
            STYLE_RESET( this.stderr_orig );                            \
          }                                                             \
          if ( this.suite.interrupted ) {                               \
            if ( n ) fprintf( this.stderr_orig, ", " );                 \
            n += this.suite.interrupted;                                \
            TEST_STYLE_EMPH();                                          \
            fprintf( this.stderr_orig, "%u got interrupted", this.suite.interrupted );\
            STYLE_RESET( this.stderr_orig );                            \
          }                                                             \
          if ( this.suite.timed_out )	{                               \
            if ( n ) fprintf( this.stderr_orig, ", " );                 \
            n += this.suite.interrupted;                                \
            TEST_STYLE_EMPH();                                          \
            fprintf( this.stderr_orig, "%u timed out", this.suite.timed_out ); \
            STYLE_RESET( this.stderr_orig );                            \
          }                                                             \
        }                                                               \
        fprintf( this.stderr_orig, ".\n" );                             \
        if ( 0 == this.env.verbose ) {                                  \
          TEST_STYLE_INFO();                                            \
          fprintf( this.stderr_orig, "Relaunch with -v for details.\n"); \
          STYLE_RESET( this.stderr_orig );                              \
        }                                                               \
      }                                                                 \
    }                                                                   \
    this.env.test_name = msg;                                           \
    if ( this.env.test_name ) {                                         \
      TEST_STYLE_INFO();                                                \
      fprintf( this.stderr_orig, "[ %12s ] ", __func__ );               \
      fprintf( this.stderr_orig, msg ? msg : " ", ##__VA_ARGS__ );      \
      STYLE_RESET( this.stderr_orig );                                  \
      fprintf( this.stderr_orig, "\n" );                                \
    }                                                                   \
    elapsed_nsec();                                                     \
    this.suite.total       = 0;                                         \
    this.suite.passed      = 0;                                         \
    this.suite.failed      = 0;                                         \
    this.suite.interrupted = 0;                                         \
    this.suite.untested    = 0;                                         \
    this.suite.aborted     = 0;                                         \
    this.suite.timed_out   = 0;                                         \
  } while ( 0 )

#define test_suite( ... ) test_suite__( __VA_ARGS__ )

  int    test_oracle_start__( char *func, int line, FILE *source, char *source_string );
#define  test_oracle_start( src ) test_oracle_start__( (char*)__func__, __LINE__, src, #src )
  int    test_oracle_check__( char *msg, char *oracle, ... );
#define  test_oracle_check( msg, ... ) test_oracle_check__( (msg), __VA_ARGS__ , "Oracle", "" )

  int    test_oracle_check_file__( char *msg, char *oracle, ... );
#define  test_oracle_check_file( msg, ... ) this.env.oracle_line = __LINE__ , test_oracle_check_file__( (msg), __VA_ARGS__ , "Oracle", "" )

#define  test_implementation( test, impl )          \
  unit_test( argc, argv );                          \
  test_suite( "Validating implementation "#impl );  \
  test( impl );                                     \
  exit( EXIT_SUCCESS )

#include <unitest/clock.h>

#ifdef __cplusplus
}
#endif

#endif /* _UNITEST_H_ */
