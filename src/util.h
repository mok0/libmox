/*
   $Id$

   Prototype definitions for util.c
   Morten Kjeldgaard, august 1995
*/

#ifndef _MOX_UTIL_H
#define _MOX_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

char *strleft (char *);
char *strtrim (char *);
char *strlower (char *);
char *strupper (char *);
char *mytime();
char *myuser();
double nicenum(double, int);
char *eatpath();
char *eatext();
void read_matrix_file(int[37][37]);

#ifdef __cplusplus
}
#endif

#endif /* util.h */

/*
  Local variables:
  mode: font-lock
  End:
*/
