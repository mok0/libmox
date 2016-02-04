/*
    This file is a part of moxlib, a utility library.
    Copyright (C) 1995-2007 Morten Kjeldgaard

    This program is free software: you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
   Datablock types: i = int, r = real, t = text, x = coordinate,
   h = int coordinate, b = byte, pointer.
*/

#ifndef _database_h
#define _database_h

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

Database *DBcreate(const char *);		/* create a new database */
void DBlist(Database *);			/* list contents of database */
Datablock *dbcreate(Database *, const char *, char, int);		/* add new empty datablock */
Datablock *dbpointer(Database *, const char *);		/* return pointer to named datablock */
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
