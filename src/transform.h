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

#ifndef _transform_h
#define _transform_h

#include "veclib.h"

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
void transform_destroy();
#define _TRANSFORM_H

#ifdef __cplusplus
}
#endif

#endif /* transform.h */
