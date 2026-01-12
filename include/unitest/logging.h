/**
 * @file logging.h
 * @author François Cayre <cayre@uvolante.org>
 * @date Fri Apr 21 15:36:53 2023
 * @brief Logging.
 *
 * Logging.
 */

#ifndef _LOGGING_H_
#define _LOGGING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>

#define STYLED( fp, style, ... ) do {           \
    STYLE_##style(fp);                          \
    ret += fprintf( fp, __VA_ARGS__ );          \
    STYLE_RESET( fp );                          \
  } while ( 0 )

#ifndef NDEBUG
#define DEBUG( fmt, ... )                                   \
  MSG_FMT( 0, " DEBUG ", 105, ST_BOLD, fmt, ##__VA_ARGS__ )
#else
#define DEBUG( fmt, ... )
#endif
#define VERBOSE( cond, fmt, ... ) if ( cond )                   \
    MSG_FMT( 0, "VERBOSE", 34, ST_FAINT, fmt, ##__VA_ARGS__ )
#define INFO( fmt, ... )                                    \
  MSG_FMT( 0, "  INFO ", 147, ST_BOLD, fmt, ##__VA_ARGS__ )
#define WARNING( fmt, ... )					\
  MSG_FMT( 0, "WARNING", 208, ST_BOLD, fmt, ##__VA_ARGS__ )
#define ERROR( fmt, ... )					\
  MSG_FMT( 0, " ERROR ", 196, ST_BOLD, fmt, ##__VA_ARGS__ )
#define FATAL( fmt, ... )					\
  MSG_FMT( 1, " FATAL ", 160, ST_BOLD";"ST_BLINK";"ST_ITALIC, fmt, ##__VA_ARGS__ )

#define ST_BOLD                "1"
#define ST_FAINT               "2"
#define ST_ITALIC              "3"
#define ST_UNDERLINE           "4"
#define ST_UNDERLINE_DBL      "21"
#define ST_BLINK               "5"
#define ST_OVERLINE           "53"
#define ST_INVERSE             "7"
#define ST_DELETED             "9"

#define ANSI_ESC           "\x1b"
#define TERM_CSI      ANSI_ESC"["

#define STYLE( file, color, style )                     \
  fprintf( (file), TERM_CSI style";38;5;"#color"m" )

#define STYLE_RESET( file ) fprintf( file, TERM_CSI"0m" )

#define MSG_FMT( fatal, prompt, color, style, fmt, ... ) do {           \
    char    _buf[ BUFSIZ ];                                             \
    char     *_p = _buf;                                                \
    size_t  _len = 0;                                                   \
    size_t    _n = 0;                                                   \
    memset( _buf, 0, BUFSIZ );                                          \
    _len = snprintf( _buf, BUFSIZ, fmt, ##__VA_ARGS__ );                \
    for ( _n = _len ; '\n' == _buf[ _n-1 ] ; _n-- );                    \
    _buf[ _n ] = '\0';                                                  \
    for ( _p = _buf ; ('\n' == *_p || '\r' == *_p || '\t' == *_p) &&    \
            _p-_buf < BUFSIZ-1 ; _p++ ) {                               \
      fprintf( stderr, "%c", *_p );                                     \
    }                                                                   \
    STYLE( stderr, color, style );                                      \
    for ( _p = strtok( _p, "\n" ) ; _p ; _p = strtok( NULL, "\n" ) ) {  \
      fprintf( stderr, "[ %s ] %s", prompt, _p );                       \
      if ( _p + strlen( _p ) != _buf + _n ) fprintf( stderr, "\n" );    \
    }                                                                   \
    STYLE_RESET( stderr );                                              \
    if ( _n < _len )                                                    \
      _buf[ _n ] = '\n';                                                \
    fprintf( stderr, "%s", _buf+_n );                                   \
    if ( fatal ) abort();                                               \
  } while ( 0 )

#ifdef __cplusplus
}
#endif

#endif /* _LOGGING_H_ */
