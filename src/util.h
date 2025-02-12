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

#ifndef _mox_util_h
#define _mox_util_h

#ifdef __cplusplus
extern "C" {
#endif

  char *strleft (char *);
  char *strtrim (char *);
  char *strlower (char *);
  char *strupper (char *);
  char *mytime();
  char *myuser();
  double nicenum(double, int);
  char *eatpath();
  char *eatext();
  int streq (char *, char *);
  int strcaseeq (char *, char *);
  void die (char *);
  double d_sign (double *, double *);
  int rand_range (int);
  char * fileexists (char *, char *);

#ifdef __cplusplus
}
#endif

#endif /* _mox_util_h */
