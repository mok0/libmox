/*
   datastructures for my database routines.
   mk 950331.

   Datablock types: i = int, r = real, t = text, x = coordinate, 
   h = int coordinate, b = byte, pointer.
*/

#ifndef _DATABASE_H
#define _DATABASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>
#include <stdio.h>

typedef struct Datablock {
  char *name;
  char type;			/* i, r, d, t, x, h, b, p */
  unsigned int size;		/* no of elements */
  unsigned int bsize;		/* size in bytes */
  time_t timestamp;
  char *format;
  int status;
  struct Datablock *next;
  void *data;
  void *maxval;			/* pointer to max valid value */
  void *minval;			/* pointer to min valid value */
  unsigned int ct;		/* last used line, for text datablocks */
} Datablock;

typedef struct Database {
  char *name;
  FILE *f;
  time_t timestamp;
  int status;			/* saved, readonly */
  struct Datablock *top;	/* pointer to first datablock */
} Database;

Database *DBcreate(char *);		/* create a new database */
void DBlist(Database *);			/* list contents of database */
Datablock *dbcreate(Database *, char *, char, int);		/* add new empty datablock */
Datablock *dbpointer(Database *, char *);		/* return pointer to named datablock */
  //Datablock *dbdelete();		/* delete datablock */
  /*
void dbdiagram();
  */
double dbmin(Datablock *);
double dbmax(Datablock *);

#ifdef __cplusplus
}
#endif

#endif /* database.h */

/*
  Local variables:
  mode: font-lock
  End:
*/







