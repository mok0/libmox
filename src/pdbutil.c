/*    
    This file is a part of moxlib, a utility library.
    Copyright (C) 1997-2007 Morten Kjeldgaard  

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdbm.h>

/**
   Pass a pdb code name to the pdb index database, and return the
   filename of the pdb entry. Database must be open before calling
   this routine. mok 970120
 */
void pdb_code_to_fname (GDBM_FILE pdb_dbm, char *code, char *fname, int siz)
{
  datum key, value;

  if (!pdb_dbm) {
    fprintf (stderr, "unable to open pdb database\n");
    exit (1);
    }

  key.dptr = code;
  key.dsize = strlen(code);
  value = gdbm_fetch (pdb_dbm, key);

  if (value.dptr) {
    if (value.dsize < siz) {
    memcpy (fname, (char *)value.dptr, value.dsize); 
    fname[value.dsize] = 0;
    } else {
      fprintf (stderr, "pdb_code_to_fname: buffer too small");
      *fname = 0;
    }
  } else
    *fname = 0;
}

/* 
   Local Variables:
   mode: font-lock
   End:
*/
