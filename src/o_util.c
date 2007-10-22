/*
   $Id: outil.c 308 2006-03-28 11:37:01Z mok $

   This file contains O interface routines, to read & write datablocks etc.
   mok 950816
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mox_vector.h"
#include "transform.h"

/* 
   O_read_transform -- open a file containing some transform written by O.
   It could be a .gs_real datablock, or it could be an .lsq_rt_* datablock.
   mok 950822
*/
Transform *
O_read_transform (FILE *f)
{
  Transform *t;
  char buf[80];
  register i,j;
  double gs[27];		/* large enough for the .gs_real datablock */

  if (!f)
    return NULL;
  buf[0] = 0;
  while (buf[0] != '.')
    if (!fgets(buf, 80, f))
      return NULL;

  if (strncasecmp (buf, ".GS_REAL", 8) == 0) {
    for (i=0; i < 27; i++) {	/* read the numbers */
      j = fscanf (f, "%lf", &gs[i]);
      if (j == 0) {
	fprintf (stderr, "error reading .gs_real datablock\n");
	return NULL;
      }
    }
    t = NEWTYPE(Transform);
    t->mat.element[0][0] = gs[4];
    t->mat.element[0][1] = gs[8];
    t->mat.element[0][2] = gs[12];
    t->mat.element[1][0] = gs[5];
    t->mat.element[1][1] = gs[9];
    t->mat.element[1][2] = gs[13];
    t->mat.element[2][0] = gs[6];
    t->mat.element[2][1] = gs[10];
    t->mat.element[2][2] = gs[14];

    t->trans.x = gs[16];
    t->trans.y = gs[17];
    t->trans.z = gs[18];

    t->name = strdup (".gs_real");
    t->next = NULL;
    return t;

  } else if (strncasecmp (buf, ".LSQ_RT_", 7) == 0) {
    for (i=0; i < 12; i++) {	/* read the numbers */
      j = fscanf (f, "%lf", &gs[i]);
      if (j == 0) {
	fprintf (stderr, "error reading .lsq_rt_* datablock\n");
	return NULL;
      }
    }
    t = NEWTYPE(Transform);
    t->mat.element[0][0] = gs[0];
    t->mat.element[0][1] = gs[3];
    t->mat.element[0][2] = gs[6];
    t->mat.element[1][0] = gs[1];
    t->mat.element[1][1] = gs[4];
    t->mat.element[1][2] = gs[7];
    t->mat.element[2][0] = gs[2];
    t->mat.element[2][1] = gs[5];
    t->mat.element[2][2] = gs[8];

    t->trans.x = gs[9];
    t->trans.y = gs[10];
    t->trans.z = gs[11];
    t->name = strdup (".lsq_rt");
    t->next = NULL;
    return t;
  } else {
    fprintf (stderr, "neither a .gs_real or .lsq_rt_* datablock file\n");
    return NULL;
  }
}

/*
   destroy this transform, and all the transforms in the list.
*/
int
transform_destroy (Transform *trf)
{
  if (!trf)
    return;

  transform_destroy (trf->next);
  free (trf->name);		/* free the name string */
  free (trf);
}

/*
   create a new transform, containing the identity operation
   mok 960803
*/
Transform *
transform_create()
{
  Transform *t;

  t = NEWTYPE(Transform);
  if (!t) {
    fprintf (stderr, "memory exhausted\n");
  } else {
    memset (t, 0, sizeof(Transform));
    t->mat.element[0][0] = 1.0;
    t->mat.element[1][1] = 1.0;
    t->mat.element[2][2] = 1.0;
  }
  return t;
}

/*
   concatenate two transformations c = ab. This implemenation is rather
   stupid, but it is not convienient to store a 4x4 matrix in the 
   transformation structure. Therefore, I first copy the transformations
   into 4x4 matrices, multiply these together, and copy the result back.
   mok 960804
*/
Transform *
transform_concat (Transform *a, Transform *b, Transform *c)
{
  Matrix4 m1, m2;
  register int i, j;

  for (i=0; i< 3; i++) {
    for (j=0; j< 3; j++) {
      m1.element[i][j] = a->mat.element[i][j];
      m2.element[i][j] = b->mat.element[i][j];
    }
  }
  m1.element[3][0] = a->trans.x;
  m1.element[3][1] = a->trans.y;
  m1.element[3][2] = a->trans.z;
  m2.element[3][0] = b->trans.x;
  m2.element[3][1] = b->trans.y;
  m2.element[3][2] = b->trans.z;

  for (i=0; i < 3; i++) {
    m1.element[i][3] = m2.element[i][3] = 0.0;
  }
  m1.element[3][3] = m2.element[3][3] = 1.0;

  M4MatMul (&m1, &m2, &m1);

  for (i=0; i< 3; i++) {
    for (j=0; j< 3; j++) {
      c->mat.element[i][j] = m1.element[i][j];
    }
  }
  c->trans.x = m1.element[3][0]; 
  c->trans.y = m1.element[3][1]; 
  c->trans.z = m1.element[3][2]; 

  return c;
}

/* 
Local Variables:
mode: c
mode: font-lock
End:
*/
