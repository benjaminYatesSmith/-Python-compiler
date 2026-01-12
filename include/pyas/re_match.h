
#ifndef _RE_MATCH_H_
#define _RE_MATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <pyas/list.h> 
#include <pyas/chargroup.h>


int re_match( list_t re, char *source, char **end );

#ifdef __cplusplus
}
#endif

#endif /* _ALL_H_ */