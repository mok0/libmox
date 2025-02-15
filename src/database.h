/*
    This file is a part of libmox, a utility library.
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

/** @struct Datablock
 * @brief This structure describes a datablock
 */
typedef struct Datablock {
  char *name;                   /*!< Name of datablock */
  char type;			/*!< Type of datablock:  i, r, d, t, x, h, b, p */
  unsigned int size;		/*!< Number of elements in datablock */
  unsigned int bsize;		/*!< Size in bytes */
  time_t timestamp;             /*!< Timestamp when written */
  char *format;                 /*!< Format to print */
  int status;                   /*!< Status  */
  struct Datablock *next;       /*!< Pointer to next datablock */
  void *data;                   /*!< Pointer to data */
  void *maxval;			/*!< Pointer to max valid value */
  void *minval;			/*!< Pointer to min valid value */
  unsigned int ct;		/*!< Last used line, for text datablocks */
} Datablock;

/** @struct Database
 * @brief This structure describes a database consisting of Datablocks.
 */
typedef struct Database {
  char *name;                   /*!< Name of database */
  FILE *f;                      /*!< File pointer  */
  time_t timestamp;             /*!< Timestamp */
  int status;			/*!< Status saved, readonly */
  struct Datablock *top;	/*!< pointer to first datablock */
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
