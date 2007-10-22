
#ifndef _transform_h
#define _transform_h

#include "mox_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Transform {	/* Transformation */
  char *name;
  Matrix3 mat;
  Vector3 trans;
  struct Transform *next;
} Transform;

Transform *O_read_transform();
Transform *transform_create();
Transform *transform_concat();
int transform_destroy();
#define _TRANSFORM_H

#ifdef __cplusplus
}
#endif

#endif /* transform.h */

/* 
Local Variables:
mode: c
mode: font-lock
End:
*/
