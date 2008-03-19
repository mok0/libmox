/*
    This file is a part of moxlib, a utility library.
    Copyright (C) 1995-2008 Morten Kjeldgaard

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

/** @file veclib.c
   @brief Routines to manipulate vectors and matrices
   @author Morten Kjeldgaard
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "veclib.h"

/**
   Return squared length of input vector.
*/
double v3_squaredlength(Vector3 *a)
{
  return (a->x * a->x)+(a->y * a->y)+(a->z * a->z);
}

/**
   Compute length of input vector.
*/
double v3_length(Vector3 *a)
{
  return sqrt((a->x * a->x)+(a->y * a->y)+(a->z * a->z));
}

/**
   Negate the input vector and return it.
*/
Vector3 *v3_negate(Vector3 *v)
{
  v->x = -v->x;  v->y = -v->y;  v->z = -v->z;
  return v;
}

/**
   Normalize the input vector and return it.
*/
Vector3 *v3_normalize(Vector3 *v)
{
  double len = v3_length(v);
  if (len != 0.0) { v->x /= len;  v->y /= len; v->z /= len; }
  return v;
}

/**
   Scale the input vector to the new length and return it.
*/
Vector3 *v3_scale(Vector3 *v, double newlen)
{
  double len = v3_length(v);
  if (len != 0.0) {
    v->x *= newlen/len;   v->y *= newlen/len;  v->z *= newlen/len;
  }
  return v;
}

/**
   Compute 3D vector sum c = a+b.
*/
Vector3 *v3_add(Vector3 *a, Vector3 *b, Vector3 *c)
{
  c->x = a->x+b->x;  c->y = a->y+b->y;  c->z = a->z+b->z;
  return c;
}

/**
   Return 3D vector difference c = a-b.
*/
Vector3 *v3_sub(Vector3 *a, Vector3 *b, Vector3 *c)
{
  c->x = a->x-b->x;  c->y = a->y-b->y;  c->z = a->z-b->z;
  return c;
}

/**
   Return 2D vector difference c = a-b.
*/
Vector2 *v2_sub(Vector2 *a, Vector2 *b, Vector2 *c)
{
  c->x = a->x-b->x;  c->y = a->y-b->y;
  return c;
}

/**
   Return the dot product of vectors a and b.
*/
double v3_dot(Vector3 *a, Vector3 *b)
{
  return (a->x*b->x)+(a->y*b->y)+(a->z*b->z);
}

/**
   Return the dot product of vectors a and b.
*/
double v2_dot(Vector2 *a, Vector2 *b)
{
  return (a->x*b->x)+(a->y*b->y);
}

/**
   Linearly interpolate between vectors by an amount alpha and return
   the resulting vector. When alpha=0, result=lo. When alpha=1,
   result=hi.
*/

Vector3 *v3_lerp(Vector3 *lo, Vector3 *hi, double alpha, Vector3 *result)
{
  result->x = LERP(alpha, lo->x, hi->x);
  result->y = LERP(alpha, lo->y, hi->y);
  result->z = LERP(alpha, lo->z, hi->z);
  return result;
}


/**
   Make a linear combination of two vectors and return the
   result.
   @return (a * ascl) + (b * bscl)
*/
Vector3 *v3_combine (Vector3 *a, Vector3 *b, Vector3 *result, double ascl, double bscl)
{
  result->x = (ascl * a->x) + (bscl * b->x);
  result->y = (ascl * a->y) + (bscl * b->y);
  result->z = (ascl * a->z) + (bscl * b->z);

  return result;
}


/**
   Multiply a 3D vector by a scalar return the result.
*/
Vector3 *v3_mul (Vector3 *a, double b, Vector3 *result)
{
  result->x = a->x * b;
  result->y = a->y * b;
  result->z = a->z * b;
  return result;
}

/**
   Divide a 3D vector by a scalar return the result.
*/
Vector3 *v3_div (Vector3 *a, double b, Vector3 *result)
{
  if (b > EPS) {
    result->x = a->x/ b;
    result->y = a->y/ b;
    result->z = a->z/ b;
  }
  return result;
}

/**
   Return the distance between two points.
*/
double v3_distance(Point3 *a, Point3 *b)
{
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  double dz = a->z - b->z;
  return sqrt((dx*dx)+(dy*dy)+(dz*dz));
}

/**
   Return the squared distance between two points.
*/
double v3_distancesquared(Point3 *a, Point3 *b)
{
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  double dz = a->z - b->z;
  return (dx*dx)+(dy*dy)+(dz*dz);
}

/**
   Return the distance between two 2D points.
*/
double v2_distance(Point2 *a, Point2 *b)
{
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  return sqrt((dx*dx)+(dy*dy));
}

/**
   Return the squared distance between two 2D points.
*/
double v2_distancesquared(Point2 *a, Point2 *b)
{
  double dx = a->x - b->x;
  double dy = a->y - b->y;
  return (dx*dx)+(dy*dy);
}

/**
   Return the vector cross product c = a X b.
*/
Vector3 *v3_cross(Vector3 *a, Vector3 *b, Vector3 *c)
{
  c->x = (a->y*b->z) - (a->z*b->y);
  c->y = (a->z*b->x) - (a->x*b->z);
  c->z = (a->x*b->y) - (a->y*b->x);
  return c;
}

/**
   Create, initialize, and return a new vector.
*/
Vector3 *v3_new(double x, double y, double z)
{
  Vector3 *v = NEWTYPE(Vector3);
  v->x = x;  v->y = y;  v->z = z;
  return v;
}

/**
   Create, initialize, and return a duplicate vector.
*/
Vector3 *v3_duplicate(Vector3 *a)
{
  Vector3 *v = NEWTYPE(Vector3);
  v->x = a->x;  v->y = a->y;  v->z = a->z;
  return v;
}


/**
   PREmultiply a point by a matrix and return the transformed
   point.
*/
Point3 *v3_premulpointbymatrix(Point3 *pin, Matrix3 *m, Point3 *pout)
{
  pout->x = (pin->x * m->element[0][0]) + (pin->y * m->element[0][1]) +
    (pin->z * m->element[0][2]);
  pout->y = (pin->x * m->element[1][0]) + (pin->y * m->element[1][1]) +
    (pin->z * m->element[1][2]);
  pout->z = (pin->x * m->element[2][0]) + (pin->y * m->element[2][1]) +
    (pin->z * m->element[2][2]);
  return pout;
}

/**
   Multiply a point by a projective matrix and return the transformed point.
*/
Point3 *v3_mulpointbyprojmatrix(Point3 *pin, Matrix4 *m, Point3 *pout)
{
  double w;
  pout->x = (pin->x * m->element[0][0]) + (pin->y * m->element[1][0]) +
    (pin->z * m->element[2][0]) + m->element[3][0];
  pout->y = (pin->x * m->element[0][1]) + (pin->y * m->element[1][1]) +
    (pin->z * m->element[2][1]) + m->element[3][1];
  pout->z = (pin->x * m->element[0][2]) + (pin->y * m->element[1][2]) +
    (pin->z * m->element[2][2]) + m->element[3][2];
  w =    (pin->x * m->element[0][3]) + (pin->y * m->element[1][3]) +
    (pin->z * m->element[2][3]) + m->element[3][3];
  if (w != 0.0) { pout->x /= w;  pout->y /= w;  pout->z /= w; }
  return(pout);
}


/**
   Multiply together 4x4 matrices c = ab. Matrix c can share storage
   location with a and/or b.
*/
Matrix4 *m4_matmul(Matrix4 *a, Matrix4 *b, Matrix4 *c)
{
  register int i, j, k;
  Matrix4 tmp;

  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      tmp.element[i][j] = 0.0;
      for (k=0; k<4; k++)
	tmp.element[i][j] += a->element[i][k] * b->element[k][j];
    }
  }
  for (i=0; i<4; i++) {
    for (j=0; j<4; j++) {
      c->element[i][j] = tmp.element[i][j];
    }
  }
  return c;
}

/**
   Multiply together 4x4 matrices c = ab. Matrix c can share storage
   location with a and/or b.
*/
Matrix3 *m3_matmul(Matrix3 *a, Matrix3 *b, Matrix3 *c)
{
  register int i, j, k;
  Matrix3 tmp;

  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) {
      tmp.element[i][j] = 0.0;
      for (k=0; k<3; k++)
	tmp.element[i][j] += a->element[i][k] * b->element[k][j];
    }
  }
  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) {
      c->element[i][j] = tmp.element[i][j];
    }
  }
  return c;
}

/**
   Given 4 points, calculate the torsion angle.
*/
double v3_torsion(Point3 *pa, Point3 *pb, Point3 *pc, Point3 *pd)
{
  double torang;
  Vector3 ab, cb, db, u, v, w;

  v3_sub(pa, pb, &ab);		/* ab = a-b */
  v3_sub(pc, pb, &cb);		/* cb = c-b */
  v3_sub(pd, pb, &db);		/* db = d-b */

  v3_cross(&ab, &cb, &u);
  v3_cross(&db, &cb, &v);

  torang = v3_angle(&u, &v);
  v3_cross(&u, &v, &w);
  if (fabs(v3_angle(&cb, &w)) <= 0.1)  /* this is either 0 or 180 */
    return torang;
  else
    return -torang;
}


/**
   Return angle in degrees between two 3D vectors.
*/
double v3_angle (Vector3 *v1, Vector3 *v2)
{

  double value;
  value = v3_dot(v1,v2)/sqrt(v3_dot(v1,v1)*v3_dot(v2,v2));
  value = (value > 1.0) ? 1.0 : value;
  value = (value < -1.0) ? -1.0 : value;
  return acos(value) * RTOD;

}

/**
   Return angle in degrees between two 2D vectors.
*/
double v2_angle (Vector2 *v1, Vector2 *v2)
{
  double value;

  value = v2_dot(v1,v2)/sqrt(v2_dot(v1,v1)*v2_dot(v2,v2));
  value = (value > 1.0) ? 1.0 : value;
  value = (value < -1.0) ? -1.0 : value;
  return acos(value) * RTOD;
}

/**
   Set all elements in 3x3 matrix to zero.
*/
Matrix3 *m3_zero (Matrix3 *m)
{
  register int i, j;
  for (i=0; i<3; i++)
    for (j=0; j<3; j++)
      m->element[i][j] = 0.0;
  return m;
}

/**
   Transpose a 4x4 matrix in place.
*/
Matrix4 *m4_transpose (Matrix4 *a)
{
  register int i, j;
  double tmp;

  for (i=0; i<4; i++) {
    for (j=i+1; j<4; j++) {
      tmp = a->element[i][j];
      a->element[i][j] = a->element[j][i];
      a->element[j][i] = tmp;
    }
  }
  return a;
}

/**
   Transpose a 3x3 matrix in place.
*/
Matrix3 *m3_transpose (Matrix3 *a)
{
  register int i, j;
  double tmp;

  for (i=0; i<3; i++) {
    for (j=i+1; j<3; j++) {
      tmp = a->element[i][j];
      a->element[i][j] = a->element[j][i];
      a->element[j][i] = tmp;
    }
  }
  return a;
}
   

/**
   Create, initialize, and return a duplicate 3x3 matrix.
*/
Matrix3 *m3_duplicate(Matrix3 *a)
{
  Matrix3 *m = NEWTYPE(Matrix3);
  memcpy(m, a, sizeof(Matrix3));
  return m;
}

/**
   Create, initialize, and return a duplicate 4x4 matrix.
*/
Matrix4 *m4_duplicate(Matrix4 *a)
{
  Matrix4 *m = NEWTYPE(Matrix4);
  memcpy(m, a, sizeof(Matrix4));
  return m;
}


/**
   Modified Gram-Schmidt Orthogonalization by Columns.  This routine
   orthogonalizes a n by n square matrix using the modified
   Gram-Schmidt procedure. If the input matrix has a singularity,
   errcod is returned with a non-zero value. @date mok 950815, translated
   from my old fortran code 900328.
*/

Matrix3 *m3_mgs (Matrix3 *m)
{
  int i, ii, ik, ido, j, n;
  double d, dot, dnorm;

  n = 3;                        /* dimension of matrix */
  for (i = 0; i < n-2; i++) {
    dnorm = 0.0;
    for (ii = 0; ii < n; ii++)
      dnorm = dnorm + SQR(m->element[ii][i]);

    if (dnorm < EPS)
      return NULL;

    ido = i+1;
    
    for (j = ido; j < n; j++) {
      dot = 0.0;
      for (ik = 0; ik < n; ik++)
        dot = dot + m->element[ik][i] * m->element[ik][j];
      for (ik = 0; ik < n; ik++)
        m->element[ik][j] = m->element[ik][j] - m->element[ik][i] * dot/dnorm;
    }
  }

  /** normalize the n x n matrix by columns */

  for (j = 0; j < n; j++) {
    d = 0.0;
    for (i = 0; i < n; i++)
      d = d + SQR(m->element[i][j]);
    d = sqrt(d);
    for (i = 0; i < n; i++)
      m->element[i][j] = m->element[i][j]/d;
  }

  return m;
}

/**
   Dump a 3x3 matrix to stderr.
*/
void m3_print (char *str, Matrix3 *mat)
{
  char buf[80];
  int i;

  fprintf (stderr,"=========\n");
  sprintf (buf,"%s\n",str); 
  fprintf (stderr, "%s", buf);
  for (i = 0; i < 3; i++) {
    sprintf (buf, "%.4f %.4f %.4f\n",
	     mat->element[i][0],
	     mat->element[i][1],
	     mat->element[i][2]);
    fprintf (stderr, "%s", buf);
  }
}

/**
   Dump a 4x4 matrix to stderr.
*/
void m4_print (char *str, Matrix4 *mat)
{
  char buf[80];
  int i;

  fprintf (stderr,"=========\n");
  sprintf (buf,"%s\n",str); 
  fprintf (stderr, "%s", buf);
  for (i = 0; i < 4; i++) {
    sprintf (buf, "%.4f %.4f %.4f %.4f\n",
	     mat->element[i][0],
	     mat->element[i][1],
	     mat->element[i][2],
	     mat->element[i][3]);
    fprintf (stderr, "%s", buf);
  }
}


/**
   Decode a 3x3 rotation matrix into a scale, and rotations around
   x, y and z axes. Positive direction version. @date mok 950815,
   translated from my old Fortran code.
*/
void m3_mat_to_rot (Matrix3 *m, double *alpha, double *beta, double *gamma)
{
  double aa, bb, gg;

  *alpha = *beta = *gamma = 0.0;

  if (!m)
    return;

  m3_mgs(m);			/* Normalize matrix */
  if (fabs (m->element[0][2]) < 0.999999) {
    gg = atan2 (m->element[0][1], m->element[0][0]);
    aa = atan2 (m->element[1][2], m->element[2][2]);

    /* make sure we don't divide by zero */
    if (fabs(aa) > 0.7854 && fabs(aa) < 2.3562) {
      bb = atan2 (- m->element[0][2], m->element[1][2]/sin(aa));
    } else {
      bb = atan2 (- m->element[0][2], m->element[2][2]/cos(aa));
    }
  } else {
    /* cos(bb) is close to zero, so matrix reduces */
    gg = 0.0;			/* we set it to zero */
    aa = atan2(m->element[1][0], m->element[1][1]);
    bb = MIN(fabs(m->element[0][2]), 1.0);
    bb = copysign(bb, m->element[0][2]); /* bb is now 1.0 or -1.0 */
    bb = asin(bb);		/* bb is now 90 or -90 */
  }
  
  /* convert to degrees */

  *alpha = -aa * RTOD;
  *beta = -bb * RTOD;
  *gamma = -gg * RTOD;
}

/**
  Return the scale factor (zoom) on the 3x3 matrix.
*/
double m3_toscale (Matrix3 *m)
{
  double x, y, z;

  if (!m)
    return 0.0;

  x = m->element[0][0] + m->element[0][1] + m->element[0][2];
  y = m->element[1][0] + m->element[1][1] + m->element[1][2];
  z = m->element[2][0] + m->element[2][1] + m->element[2][2];
  return sqrt((SQR(x) + SQR(y) + SQR(z))/3.0);
}


/**
   Given the axis ('x', 'y', 'z') and the angle of rotation around
   this axis, return the 3x3 rotation matrix.  mok 930217, revised
   960803, to agree with M3toRot on angle convention.
*/
Matrix3 *m3_rot_to_mat (Matrix3 *m, char axis, double angle)
{
  double ca, sa, ang;

  ang = angle*M_PI/180.0;
  ca = cos (ang); sa = sin(ang);
  memset (m, 0, sizeof (Matrix3)); /* set all elements to 0 */

  switch (axis) {
  case 'Z':
  case 'z':
    m->element[0][0] = ca;
    m->element[0][1] = -sa;
    m->element[1][0] = sa;
    m->element[1][1] = ca;
    m->element[2][2] = 1.0;
    break;
  case 'Y':
  case 'y':
    m->element[0][0] = ca;
    m->element[0][2] = sa;
    m->element[1][1] = 1.0;
    m->element[2][0] = -sa;
    m->element[2][2] = ca;
    break;
  case 'X':
  case 'x':
    m->element[0][0] = 1.0;
    m->element[1][1] = ca;
    m->element[1][2] = -sa;
    m->element[2][1] = sa;
    m->element[2][2] = ca;
    break;
  default:
    fprintf (stderr, "bogus axis\n");
  }
  return m;
}

/**
  Compute the midpoint between vectors a and b.
  @date 19971109
*/
Vector3 *v3_mid(Vector3 *a, Vector3 *b, Vector3 *c)
{
  c->x = (a->x+b->x)/2.0;  c->y = (a->y+b->y)/2.0;  c->z = (a->z+b->z)/2.0;
  return c;
}

/**
   Add a scaled vector to a, and return the result.
   @result a  + (b * bscl)
*/
Vector3 *v3_addscl (Vector3 *a, Vector3 *b, Vector3 *result, double bscl)
{
  result->x = a->x + (bscl * b->x);
  result->y = a->y + (bscl * b->y);
  result->z = a->z + (bscl * b->z);

  return result;
}

/**
  Set 2D vector elements to a value.
*/
Vector2 *v2_set(Vector2 *v, double a, double b)
{
  v->x = a; v->y = b; 
  return v;
}

/**
  Set 3D vector elements to a value.
*/
Vector3 *v3_set(Vector3 *v, double a, double b, double c)
{
  v->x = a; v->y = b; v->z = c;
  return v;
}


/**
   Set up an orthogonalization matrix from a unit cell. Use PDB
   standard.  Fractional coords to cartesian, isw <= 0 Cartesian
   coords to fractional, isw > 0.
 */
Matrix3 *orthog(float *cell, Matrix3 *m, int isw)
{
  register int i;
  register double *fp;
  double abcs[3], sabg[3];
  double cabgs[3], sabgs1, vol, cabg[3];

  for (i = 0; i < 3; ++i) {
    cabg[i] = cos((double)cell[i + 3] * DTOR);
    sabg[i] = sin((double)cell[i + 3] * DTOR);
  }

  cabgs[0] = (cabg[1] * cabg[2] - cabg[0]) / (sabg[1] * sabg[2]);
  cabgs[1] = (cabg[2] * cabg[0] - cabg[1]) / (sabg[2] * sabg[0]);
  cabgs[2] = (cabg[0] * cabg[1] - cabg[2]) / (sabg[0] * sabg[1]);

  vol = cell[0] * cell[1] * cell[2] *
    sqrt(1.0 + 2.0 * cabg[0]*cabg[1]*cabg[2]
	 - SQR(cabg[0]) - SQR(cabg[1]) - SQR(cabg[2]));
    
  abcs[0] = cell[1] * cell[2] * sabg[0] / vol;
  abcs[1] = cell[0] * cell[2] * sabg[1] / vol;
  abcs[2] = cell[0] * cell[1] * sabg[2] / vol;

  sabgs1 = sqrt(1. - SQR(cabgs[0]));

  for (fp = &m->element[0][0], i = 0; i < 9; ++i) {
    *fp++ = 0.0;
  }

  if (isw <= 0) {
    m->element[0][0] = cell[0];
    m->element[0][1] = cabg[2]*cell[1];
    m->element[0][2] = cabg[1]*cell[2];
    m->element[1][1] = sabg[2]*cell[1];
    m->element[1][2] = -sabg[1]*cabgs[0]*cell[2];
    m->element[2][2] = sabg[1]*sabgs1*cell[2];
  } else {
    m->element[0][0] = 1.0/cell[0];
    m->element[0][1] = -cabg[2] / (sabg[2] * cell[0]);
    m->element[0][2] = -(cabg[2]*sabg[1]*cabgs[0] + cabg[1]*sabg[2]) /
      (sabg[1] * sabgs1 * sabg[2] *cell[0]);
    m->element[1][1] = 1.0 / (sabg[2] * cell[1]);
    m->element[1][2] = cabgs[0] / (sabgs1 * sabg[2] * cell[1]);
    m->element[2][2] = 1.0 / (sabg[1] * sabgs1 * cell[2]);
  }
  return m;
}


/**
  Copy a 3D vector.
*/
Vector3 *v3_copy (Vector3 *u, Vector3 *v)
{
  v->x = u->x; v->y = u->y; v->z = u->z;
  return v;
}

/**
  Copy a 2D vector.
*/
Vector2 *v2_copy (Vector2 *u, Vector2 *v)
{
  v->x = u->x; v->y = u->y;
  return v;
}

/**
  Copy a 4x4 matrix to another.
*/
Matrix4 *m4_copy (Matrix4 *n, Matrix4 *m)
{
  register int i, j;

  for (i=0; i < 4; i++)
    for (j=0; j < 4; j++)
      m->element[i][j] = n->element[i][j];
  return m;
}

/**
  Set 4x4 matrix to identity.
*/
Matrix4 *m4_ident (Matrix4 *m)
{
  m->element[0][0] = 1.0; m->element[0][1] = 0.0;  
  m->element[0][2] = 0.0; m->element[0][3] = 0.0;
  m->element[1][0] = 0.0; m->element[1][1] = 1.0;  
  m->element[1][2] = 0.0; m->element[1][3] = 0.0;
  m->element[2][0] = 0.0; m->element[2][1] = 0.0;  
  m->element[2][2] = 1.0; m->element[2][3] = 0.0;
  m->element[3][0] = 0.0; m->element[3][1] = 0.0;  
  m->element[3][2] = 0.0; m->element[3][3] = 1.0;
  return m;
}

/**
  Dump a 3D vector to stderr. Mostly for debugging.
*/
void v3_print (char *str, Vector3 *vec)
{
  fprintf (stderr,"%s: %f %f %f\n", str, vec->x, vec->y, vec->z);
}

/**
  Dump a 2D vector to stderr. Mostly for debugging.
*/
void v2_print (char *str, Vector2 *vec)
{
  fprintf (stderr, "%s: %f %f\n", str, vec->x, vec->y);
}

/**
   Multiply a point by the rotation part of a 4x4 matrix and return
   the transformed point. @date 19930216.
*/
Point3 *v3_mulpointbymatrix4 (Point3 *pin, Matrix4 *m, Point3 *pout)
{
  pout->x = (pin->x * m->element[0][0]) + (pin->y * m->element[1][0]) +
    (pin->z * m->element[2][0]);
  pout->y = (pin->x * m->element[0][1]) + (pin->y * m->element[1][1]) +
    (pin->z * m->element[2][1]);
  pout->z = (pin->x * m->element[0][2]) + (pin->y * m->element[1][2]) +
    (pin->z * m->element[2][2]);
  return(pout);
}

/*
  Local variables:
  mode: font-lock
  End:
*/
