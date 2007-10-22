/*
  $Id: pdbutil.c 261 2006-02-08 23:18:28Z mok $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdbm.h>

/*
 *  pdb_code_to_fname -- Pass a pdb code name to the pdb index database, 
 *  and return the filename of the pdb entry. Database must be open 
 *  before calling this routine.
 *  mok 970120
 */
void 
pdb_code_to_fname (GDBM_FILE pdb_dbm, char *code, char *fname, int siz)
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
