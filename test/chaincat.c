/*
    This file is a part of libmox, a utility library.
    Copyright (C) 1995-2009 Morten Kjeldgaard

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
#include <unistd.h>
#include "../src/atom.h"

/**
   chain_residue_out -- print out a residue list, given the top pointer.
   mk 950329
*/

static void chain_residue_out (Chain *chain)
{
  Residue *res;

  while (chain) {
    printf ("-----------------------------------------------------------\n");
    printf ("name: %-4s id: %c residues: %6d atoms: %6d, cg: %.2f %.2f %.2f\n",
	    chain->name, chain->id, chain->nres, chain->natoms,
	    chain->cg.x, chain->cg.y, chain->cg.z);
    printf ("%d residues with insertion codes, ", chain->nresins);
    printf ("%d atoms with alternate positions\n", chain->naltatm);
    printf ("average temperature factor %f\n", chain->bav);
    printf ("zone: %s %s\n", chain->first->name, chain->last->name);
    printf ("chain class: ");
    atm_residue_class_out (chain->chain_class);
    printf ("\n");

    res = chain->res;
    while (res) {
      printf ("%4d %c:%-4s %-3s %4d %2d %8.2f %8.2f %8.2f ",
	      res->ic, res->chain->id, res->name, res->type,
	      res->resno, res->natoms,
	      res->cg.x, res->cg.y, res->cg.z);

      atm_residue_class_out (res->res_class);
      {
	Atom *atom;
	atom = res->atoms;
	while (atom) {
	  printf (" %s", atom->name);
	  atom = atom->next;

	}
	printf ("\n");
      }
      res = res->next;
    }
    chain = chain->next;
  }
}

int main (int argc, char **argv)
{
  AtomFile *f;
  Structure *shead;
  int c, rflag = 0, zflag=0;
  extern int optind;

  while ((c = getopt (argc, argv, "r")) != EOF)
    if (c == 'r')
      rflag = 1;
    else {
      fprintf(stderr, "usage: %s [-r] pdbfile[.gz]\n", argv[0]);
      return 1;
    }

  /* We are done parsing options */

  if (optind < argc){          /* Assume input file name */
    freopen(argv[optind], "r", stdin);
  }
  f = atm_dopen_file (0, "r");

  if (!f) {
    fprintf (stderr, "could not open file %s\n", argv[optind]);
    return 2;
  }

  printf ("about to read file...");
  fflush(stdout);
  shead = atm_read_pdbfile(f);
  printf ("done.\n");
  atm_close_file(f);

  Structure *s = shead;

  while(s) {
  atm_structure_out (s);
  if (rflag)
    chain_residue_out (s->chain);
  else
    atm_chain_out(s->chain);
  s = s->next;
  }

  atm_delete_structure(shead);
  return 0;
}
