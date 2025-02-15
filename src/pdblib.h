
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

#ifndef _pdblib_h
#define _pdblib_h

# include "veclib.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @struct pdb_atom_record
 * @brief This structure describes an ATOM record from a PDB file.
 */
typedef struct pdb_atom_record {
  int serial;			/*!< serial number */
  char name[5];			/*!< name */
  char chem[3];			/*!< chemical symbol, right justified */
  char remote;			/*!< remoteness indicator */
  char branch;			/*!< branch indicator */
  char aloc;			/*!< alternate atom location id */
  unsigned char Z;		/*!< element number */
  int resno;			/*!< residue number */
  int irc;			/*!< internal residue count */
  char resname[7];		/*!< residue name */
  char restype[4];		/*!< residue type */
  char chain;			/*!< chain identifier */
  char resins;			/*!< code for insertion of residues */
  Point3 xyz;			/*!< coordinate */
  float occ;			/*!< occupancy */
  float b;			/*!< temperature factor */
  struct pdb_atom_record *next;	/*!< pointer to next atom record */
  struct pdb_atom_record *prev;	/*!< pointer to previous atom */
} pdb_atom_record;

/** @struct pdb_cryst_record
 * @brief This structure describes a CRYST record from a PDB file.
 */
typedef struct pdb_cryst_record {
  double cell[6];		/*!< cell parameters */
  char spacegroup[12];		/*!< space group name */
  int z;			/*!< molecules per asymmetric unit */
} pdb_cryst_record;

/** @struct pdb_compnd_record
 * @brief This structure describes a COMPOUND record from a PDB file.
 */
typedef struct pdb_compnd_record {
  char *text;			  /*!< Pointer to text */
  struct pdb_compnd_record *next; /*!< pointer to next record */
} pdb_compnd_record;

/** @struct pdb_ss_record
 * @brief This structure describes an SS bond from a PDB file.
 */
typedef struct pdb_ss_record {
  struct pdb_ss_record *next;   /*!< pointer to next SS record */
  int what;			/*!< 1 for helix, 2 for strand */
  int serial;			/*!< serial number of helix/strand */
  char name[4];			/*!< name of this helix/strand */
  char chain;			/*!< chain id of this helix/strand */
  char restype1[4];             /*!< residue type 1 */
  char restype2[4];             /*!< residue type 2 */
  char from[7];                 /*!< from residue */
  char to[7];                   /*!< to residue */
  int class;                    /*!< class */
} pdb_ss_record;

/** @struct PDBfile
 * @brief This structure describes a file of atom records.
 */
typedef struct PDBfile {
  char id[8];			/*!< the pdb identifier code */
  char *name;			/*!< name of the file */
  FILE *f;			/*!< file descriptor */
  int natoms;			/*!< number of atom records */
  int nres;			/*!< number of residues */
  pdb_atom_record *ATOM;	/*!< pointer to first atom record */
  pdb_cryst_record *CRYST1;	/*!< pointer to cryst record */
  pdb_compnd_record *COMPND;	/*!< pointer to first compnd record */
  struct PDBfile *next;         /*!< pointer to next file */
  struct PDBfile *prev;         /*!< pointer to previous file */
} PDBfile;

#define NULLATM (pdb_atom_record *)0L;

pdb_atom_record *pdb_get_next_atom ();
pdb_atom_record *pdb_decode_atom_record ();
pdb_atom_record *pdb_read_atoms ();
pdb_cryst_record *pdb_get_cryst_record ();
pdb_cryst_record *pdb_decode_cryst_record ();
pdb_ss_record *pdb_decode_helix_record ();
pdb_ss_record *pdb_decode_sheet_record ();
void pdb_atoms_out ();
int pdb_atom_to_z();
PDBfile *pdb_read_file();

#ifdef __cplusplus
}
#endif

#endif /* pdblib.h */
