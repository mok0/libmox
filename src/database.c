/*
   $Id$

   My database routines.
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "mox_vector.h"
#include "database.h"

/* 
   DBcreate -- create a new empty database.
   mk 950404.
*/
Database *
DBcreate(char *name)
{
  Database *DB;
  extern char *strdup();

  DB = NEWTYPE(Database);
  if (DB == NULL)
    return NULL;

  DB->name = strdup(name);
  DB->timestamp = time(NULL);
  DB->status = 0;
  DB->top = NULL;
  return DB;
}

/*
   DBlist -- list the contents of the database.
   mk 950404.
*/
void 
DBlist (Database *DB)
{
  Datablock *db;
  char *s, *str;

  db = DB->top;
  while (db) {
    str = s = ctime(&db->timestamp);
    /* get rid of the newline */
    while (*s) {
      if (*s == '\n') {
	*s = '\0';
	break;
      }
      s++;
    } 
    printf ("%c %10d %10d %30s %s\n", db->type, db->size, db->bsize, str, 
	    db->name);
    db = db->next;
  }
}

  
/* 
   dbcreate -- add new empty datablock of given name and size.
   mk 950331.
*/
Datablock *
dbcreate(Database *DB, char *name, char type, int size)
{
  Datablock *d;
  extern char *strdup();

#ifdef DEBUG
  fprintf (stderr, "creating datablock %s, type %c, size %d\n", name, type, size);
#endif
  d = NEWTYPE(Datablock);
  if (d == NULL)
    return NULL;
  d->next = DB->top;
  DB->top = d;
  d->name = strdup(name);
  d->size = size;
  d->type = type;
  d->timestamp = time(NULL);
  d->format = NULL;
  d->status = 0;
  d->ct = 0;
  d->data = NULL;
  d->maxval = NULL;
  d->minval = NULL;

  switch (type) {
  case 'i':
    if (size > 1)
      d->data = (void *)calloc(size, sizeof(int));
    d->bsize = sizeof(int)*size;
    break;
  case 'r':
  case 'f':
    if (size > 1)
      d->data = (void *)calloc(size, sizeof(float));
    d->bsize = sizeof(float)*size;
    d->type = 'f';
    break;
  case 'd':
    if (size > 1)
      d->data = (void *)calloc(size, sizeof(double));
    d->bsize = sizeof(double)*size;
    d->type = 'd';
    break;
  case 'x':
    if (size > 1)
      d->data = (void *)calloc(size, sizeof(Point3));
    d->bsize = sizeof(Point3)*size;
    break;
  case 'h':
    if (size > 1)
      d->data = (void *)calloc(size, sizeof(Point3i));
    d->bsize = sizeof(Point3i)*size;
    break;
  case 'b':
    if (size > 1)
      d->data = (void *)calloc(size, sizeof(char));
    d->bsize = sizeof(char)*size;
    break;
  case 'p':
    if (size > 1)
      d->data = (void *)calloc(size, sizeof(void *));
    d->bsize = sizeof(void *)*size;
    break;
  case 't':
    if (size > 1)
      d->data = (char **)calloc(size, sizeof(char *));
    d->bsize = sizeof(char *)*size;
    break;
  default:
    if (size > 1)
      d->data = (void *)calloc(size, sizeof(int));
    d->bsize = sizeof(int)*size;
    d->type = 'i';
  }

  if (size > 1 && !d->data)
    fprintf (stderr, "could not allocate %d %c type elements for %s\n",
	     size, type, name);

  return d;
}


/*
   dbpointer -- return pointer to data area of named datablock, NULL if 
   datablock was not found. This is a simple search from the top of the list. 
   Could be greatly improved, using a hash table or some such.
   mk 950404.
*/
Datablock *
dbpointer (Database *DB, char *name)
{
  Datablock *db;

  db = DB->top;
  while (db) {
    if (db->name){ 
    if (strcasecmp(db->name, name) == 0)
	return db;
    }
    db = db->next;
  }
  return NULL;
}

#include <math.h>		/* definition of HUGE */

/*
   dbmin -- return (as a double value) the minimum value of an integer or real
   datablock.
   mk 950406
*/
double dbmin (Datablock *db)
{
  double m;
  int i;

  if (!db)
    return 0.0;

  m = HUGE;
  if (db->type == 'i') {
    int *ip;
    ip = (int *)db->data;
    for (i=0; i<db->size; i++) {
      m = MIN (m, *ip++);
    } 

  } else if (db->type == 'f') {
    float *fp;
    fp = (float *)db->data;
    for (i=0; i<db->size; i++) {
      m = MIN (m, *fp++);
    }

  } else if (db->type == 'd') {
    double *fp;
    fp = (double *)db->data;
    for (i=0; i<db->size; i++) {
      m = MIN (m, *fp++);
    }

  } else
    m = 0.0;
  
  return m;
}


/*
   dbmax -- return (as a double value) the maximum value of an integer or real
   datablock.
   mk 950406
*/
double dbmax (Datablock *db)
{
  double m;
  int i;

  if (!db)
    return 0.0;

  m = -HUGE;
  if (db->type == 'i') {
    int *ip;
    ip = (int *)db->data;
    for (i=0; i<db->size; i++) {
      m = MAX (m, *ip++);
    } 
  }else if (db->type == 'f') {
    float *fp;
    fp = (float *)db->data;
    for (i=0; i<db->size; i++) {
      m = MAX (m, *fp++);
    }
  }else if (db->type == 'd') {
    double *fp;
    fp = (double *)db->data;
    for (i=0; i<db->size; i++) {
      m = MAX (m, *fp++);
    }
  } else
    m = 0.0;
  
  return m;
}


#ifdef TESTING

main () 
{
  Database *DB;
  Datablock *db;

  DB = DBcreate("test");

  db = dbcreate (DB, "integers", 'i', 1000);
  db = dbcreate (DB, "reals", 'r', 1000);
  db = dbcreate (DB, "coordinates", 'x', 1000);
  db = dbcreate (DB, "hkls", 'h', 1000);
  db = dbcreate (DB, "text", 't', 1000);
  db = dbcreate (DB, "bytes", 'b', 1000);
  db = dbcreate (DB, "pointers", 'p', 1000);
  db = dbcreate (DB, "mol_atom_xyz", 'x', 10);
  DBlist (DB);

}
#endif

/* 
   Local Variables:
   mode: font-lock
   End:
*/

