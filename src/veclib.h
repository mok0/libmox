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

#ifndef _veclib_h
#define _veclib_h

#ifdef __cplusplus
extern "C" {
#endif

/** @struct Point2Struct
 *  @brief This structure holds a 2D vector of doubles.
 */
typedef struct Point2Struct {	/* 2d point */
  double x;  /*!< x coordinate */
  double y;  /*!< y coordinate */
} Point2;
typedef Point2 Vector2;

/** @struct IntPoint2Struct
 *  @brief This structure holds a 2D vector of ints.
 */
typedef struct IntPoint2Struct { /* 2d integer point */
  int x;  /*!< x coordinate */
  int y;  /*!< y coordinate */
} Point2i;

/** @struct Matrix3Struct
 *  @brief This structure holds a 3D transformation matrix.
 */
typedef struct Matrix3Struct {	/* 3-by-3 matrix */
  double element[3][3];   /*!< 3D transformation matrix */
} Matrix3;

/** @struct Box2dStruct
 *  @brief This structure holds a 2D bounding box.
 */
  typedef struct Box2dStruct {	/* 2d box */
  Point2 min;  /*!< Min coordinates of 2D box */
  Point2 max;  /*!< Max coordinates of 2D box */
} Box2;

/** @struct Box2v
 *  @brief This structure holds a 2D bounding box of 2D double vectors
 */
typedef struct {		/* 2d box */
  double min[2];  /*!< Min coordinates 2D of box */
  double max[2];  /*!< Max coordinates 2D of box */
} Box2v;

/** @struct Point3Struct
 *  @brief This structure holds a 3D coordinate of doubles.
 */
typedef struct Point3Struct {	/* 3d point */
  double x;  /*!< x coordinate */
  double y;  /*!< y coordinate */
  double z;  /*!< z coordinate */
} Point3;
typedef Point3 Vector3;

/** @struct IntPoint3Struct
 *  @brief This structure holds a 3D coordinate of ints.
 */
typedef struct IntPoint3Struct { /* 3d integer point */
  int x;  /*!< x coordinate */
  int y;  /*!< y coordinate */
  int z;  /*!< z coordinate */
} Point3i;


/** @struct Matrix4Struct
 * @brief This structure holds a 4D transformation matrix
 */
  typedef struct Matrix4Struct {	/* 4-by-4 matrix */
  double element[4][4];  /*!< 4D transformation matrix */
} Matrix4;

/** @struct Box3dStruct
 * @brief This structure holds a 3D bounding box.
 */
typedef struct Box3dStruct {	/* 3d box */
  Point3 min; /*!< Min coordinates of 3D box */
  Point3 max; /*!< Max coordinates of 3D box */
} Box3;

/** @struct Box3v
 *  @brief This structure holds a 2D bounding box of 3D double vectors
 */
typedef struct {		/* 3d box */
  double min[3]; /*!< Min coordinates of 3D box */
  double max[3]; /*!< Max coordinates of 3D box */
} Box3v;

/* absolute value of a */
#define ABS(a)		(((a)<0) ? -(a) : (a))

/* round a to nearest integer towards 0 */
#define FLOOR(a)		((a)>0 ? (int)(a) : -(int)(-a))

/* round a to nearest integer away from 0 */
#define CEILING(a) \
((a)==(int)(a) ? (a) : (a)>0 ? 1+(int)(a) : -(1+(int)(-a)))

/* round a to nearest int */
#define ROUND(a)	((a)>0 ? (int)(a+0.5) : -(int)(0.5-a))

/* take sign of a, either -1, 0, or 1 */
#define ZSGN(a)		(((a)<0) ? -1 : (a)>0 ? 1 : 0)

/* take binary sign of a, either -1, or 1 if >= 0 */
#define SGN(a)		(((a)<0) ? -1 : 0)

/* transfer of sign, |a| if b >= 0, -|a| if b < 0 */
#define SIGN(a,b)       (((b)<0) ? -ABS(a) : ABS(a))

/* shout if something that should be true isn't */
#define ASSERT(x) \
if (!(x)) fprintf(stderr," Assert failed: x\n");

/* square a */
#define SQR(a)		((a)*(a))

/* find minimum of a and b */
#define MIN(a,b)	(((a)<(b))?(a):(b))

/* find maximum of a and b */
#define MAX(a,b)	(((a)>(b))?(a):(b))

/* swap a and b (see Gem by Wyvill) */
#define SWAP(a,b)	{ a^=b; b^=a; a^=b; }

/* linear interpolation from l (when a=0) to h (when a=1)*/
/* (equal to (a*h)+((1-a)*l) */
#define LERP(a,l,h)	((l)+(((h)-(l))*(a)))

/* clamp the input to the specified range */
#define CLAMP(v,l,h)	((v)<(l) ? (l) : (v) > (h) ? (h) : v)

/* create a new instance of a structure (see Gem by Hultquist) */
#define NEWSTRUCT(x)	(struct x *)(malloc((unsigned)sizeof(struct x)))

/* create a new instance of a type */
#define NEWTYPE(x)	(x *)(malloc((unsigned)sizeof(x)))

#ifndef PI
# define PI		3.141592	/* the venerable pi */
#endif
#define PITIMES2	6.283185	/* 2 * pi */
#define PIOVER2		1.570796	/* pi / 2 */
#define E		2.718282	/* the venerable e */
#define SQRT2		1.414214	/* sqrt(2) */
#define SQRT3		1.732051	/* sqrt(3) */
#define GOLDEN		1.618034	/* the golden ratio */
#define DTOR		0.017453	/* convert degrees to radians */
#define RTOD		57.29578	/* convert radians to degrees */
#define EPS             1.0e-12         /* in principle zero */

#define TRUE		1
#define FALSE		0
#define ON		1
#define OFF 		0

typedef int boolean;		/* boolean data type */

double v3_squared_length(Vector3 *);
double v3_length(Vector3 *);
double v3_dot(Vector3 *, Vector3 *);
double v2_dot(Vector2 *, Vector2 *);
double v3_distance(Point3 *, Point3 *);
double v2_distance(Point2 *, Point2 *);
double v3_distance_squared(Point3 *, Point3 *);
double v2_distance_squared(Point2 *, Point2 *);
double v3_angle(Vector3 *, Vector3 *);
double v2_angle(Vector2 *, Vector2 *);
double v3_torsion(Point3 *, Point3 *, Point3 *, Point3 *);
double m3_to_scale(Matrix3 *);

Vector2 *v2_set(Vector2 *, double, double);
Vector2 *v2_sub(Vector2 *, Vector2 *, Vector2 *);
Vector2 *v2_copy (Vector2 *, Vector2 *);

Vector3 *v3_normalize(Vector3 *);
Vector3 *v3_scale(Vector3 *, double);
Vector3 *v3_add(Vector3 *, Vector3 *, Vector3 *);
Vector3 *v3_sub(Vector3 *, Vector3 *, Vector3 *);
Vector3 *v3_lerp(Vector3 *, Vector3 *, double, Vector3 *);
Vector3 *v3_combine(Vector3 *, Vector3 *, Vector3 *, double, double);
Vector3 *v3_mul(Vector3 *, double, Vector3 *);
Vector3 *v3_cross(Vector3 *, Vector3 *, Vector3 *);
Vector3 *v3_new(double, double, double);
Vector3 *v3_duplicate(Vector3 *);
Vector3 *v3_mid(Vector3 *, Vector3 *, Vector3 *);
Vector3 *v3_set(Vector3 *, double, double, double);
Vector3 *v3_negate(Vector3 *);
Vector3 *v3_add_scaled (Vector3 *, Vector3 *, Vector3 *, double);
Vector3 *v3_div (Vector3 *, double, Vector3 *);
Vector3 *v3_copy (Vector3 *, Vector3 *);

Point3 *v3_mul_point_by_matrix();
Point3 *v3_mul_point_by_proj_matrix(Point3 *, Matrix4 *, Point3 *);
Point3 *v3_pre_mul_point_by_matrix(Point3 *, Matrix3 *, Point3 *);

Matrix3 *m3_matmul(Matrix3 *, Matrix3 *, Matrix3 *);
Matrix3 *m3_zero(Matrix3 *);
Matrix3 *m3_mgs(Matrix3 *);
Matrix3 *m3_transpose(Matrix3 *);
Matrix3 *orthog(float *, Matrix3 *, int);
Matrix3 *m3_rot_to_mat(Matrix3 *, char, double);
Matrix3 *m3_duplicate(Matrix3 *);

Matrix4 *m4_duplicate(Matrix4 *);
Matrix4 *m4_matmul(Matrix4 *, Matrix4 *, Matrix4 *);
Matrix4 *m4_transpose(Matrix4 *);
Matrix4 *m4_ident (Matrix4 *);
Matrix4 *m4_copy();

void m3_mat_to_rot(Matrix3 *, double *, double *, double *);
void m3_print(char *, Matrix3 *);


#ifdef __cplusplus
}
#endif

#endif /* _veclib_h */
