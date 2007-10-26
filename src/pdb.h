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

#ifndef _pdb_h
#define _pdb_h

#ifdef __cplusplus
extern "C" {
#endif

enum pdbrecord_type {HEADER, REMARK, COMPND, CRYST1, ATOM, HETATM,
		     HELIX, SHEET, ANISOU, TER, ENDFILE};

typedef struct pdb_atom_record {
  int serial;			/* serial number */
  char name[5];			/* name */
  char chem[3];			/* chemical symbol, right justified */
  char remote;			/* remoteness indicator */
  char branch;			/* branch indicator */
  char aloc;			/* alternate atom location id */
  unsigned char z;		/* element number */
  int resno;			/* residue number */
  int irc;			/* internal residue count */
  char resname[7];		/* residue name */
  char restype[4];		/* residue type */
  char chain;			/* chain identifier */
  char resins;			/* code for insertion of residues */
  Point3 xyz;			/* coordinate */
  float occ;			/* occupancy */
  float b;			/* temperature factor */
  struct pdb_atom_record *next;	/* pointer to next atom */
  struct pdb_atom_record *prev;	/* pointer to previous atom */
} pdb_atom_record;

typedef struct pdb_aniso_record {
  int serial;			/* serial number */
  char name[5];			/* name */
  char chem[3];			/* chemical symbol, right justified */
  char remote;			/* remoteness indicator */
  char branch;			/* branch indicator */
  char aloc;			/* alternate atom location id */
  unsigned char z;		/* element number */
  int resno;			/* residue number */
  char resname[7];		/* residue name */
  char restype[4];		/* residue type */
  char chain;			/* chain identifier */
  char resins;			/* code for insertion of residues */
  float aniso[6];		/* anisotropic temperature factor */
} pdb_aniso_record;

/* prototypes */

static pdb_atom_record *decode_pdb_atom();
static pdb_aniso_record *decode_pdb_aniso();
static void decode_pdb_atom_id();

#ifdef __cplusplus
}
#endif

#endif /* pdb.h */

/*
  Local variables:
  mode: font-lock
  End:
*/
