/*
    This file is a part of libmox, a utility library.
    Copyright (C) 1995-2016 Morten Kjeldgaard

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

#ifndef _lqsort_h
#define _lqsort_h

#ifdef __cplusplus
extern "C" {
#endif

  void lqsort(void *lstp, int (*cmp)());

#ifdef __cplusplus
}
#endif

#endif /* _lqsort_h */
