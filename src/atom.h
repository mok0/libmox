/*    
    This file is a part of moxlib, a utility library.
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

#ifndef _mox_atom_h
#define _mox_atom_h

#ifdef ZLIB
#  include <zlib.h>
#endif

# include "veclib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* define USEPRIME if you want to use primes in atom names on nucleic acids 
   instead of the standard asterisk */

#define USEPRIME

enum class_type { PROTEIN, AMINOACID, CIS, NUCLEIC_ACID, DNA, RNA,
		  PURINE, PYRIMIDINE, SOLVENT, HETERO, NUCLEOTIDE, 
		  RNABASE, DNABASE, POLYPHOSPHATE };

enum atom_type { ATOM_CA, ATOM_N, ATOM_C, ATOM_O, ATOM_P, ATOM_O2P, ATOM_O3P};

/* define some bit manipulation macros! */
#define setbit(a,i)  ((a)   |=  (1<<(i)))
#define clrbit(a,i)  ((a)   &= ~(1<<(i)))
#define isset(a,i)     ((a) &   (1<<(i)))
#define isclear(a,i) (!((a) &   (1<<(i))))

typedef struct {
  char *name;
#ifdef ZLIB
  gzFile *File;
#else
  FILE *File;
#endif
  char *mode;
} AtomFile;

typedef struct Atom {
  char name[6];			/* atom name */
  char ins;			/* insertion id */
  unsigned char z;		/* element number */
  Point3 xyz;			/* coordinate */
  float b, occ;			/* temperature factor, occupancy*/
  float *aniso;			/* pointer to anisotropic B factor */
  int ic;			/* internal atom count */
  unsigned long flag;		/* flag to store properties of this atom */
  struct Atom *next;		/* Pointer to next atom */
  struct Atom *prev;		/* pointer to previous atom */
  struct Atom *alt;		/* ptr to alternate position(s) of this atom */
  struct Residue *res;		/* pointer to parent residue */
} Atom;

typedef struct Residue {
  char name[12];		/* residue name */
  char type[6];			/* residue type */
  int resno;			/* residue number */
  char resins;			/* code for insertion of residues */
  int ic;			/* internal residue count */
  Point3 cg;			/* center-of-gravity */
  float rad;			/* radius from cg that includes all atoms */
  float bav;			/* average B-factor of this residue */
  unsigned char naltatm;	/* number of atoms with alternate positions */
  struct Chain *chain;		/* pointer to chain */
  unsigned char natoms;		/* number of atoms in this residue*/
  struct Atom *atoms;		/* pointer to list of atoms */
  struct Atom *ca;		/* pointer to central atom */
  unsigned long flag;	        /* flag to store properties of this residue */
  unsigned long res_class;
  struct Residue *next;		/* pointer to next res */
  struct Residue *prev;		/* pointer to prev res */
  struct Residue *bond_next;	/* if bonded to next res, = next else null */
  struct Residue *bond_prev;	/* if bonded to prev res, = prev else null */
} Residue;

typedef struct Chain {
  char name[6];			/* name of this chain */
  char id;			/* id of this chain */
  int nres;			/* number of residues */
  int natoms;			/* number of atoms in this chain */
  int nhetatoms;		/* number of pdb style hetatoms in this chain */
  int nresins;			/* number of inserted residues */
  int naltatm;			/* number of atoms with alternate positions */
  int ic;			/* internal count number of this chain */
  Point3 cg;			/* center-of-gravity */
  float rad;			/* radius from cg that includes all atoms */
  float bav;			/* average B-factor of this chain */
  unsigned long chain_class;	/* class of this chain */
  int hetcount;			/* non-water het groups in this chain */
  Residue *res;			/* pointer to residue list */
  Residue *first;		/* pointer to first residue (alias for *res) */
  Residue *last;		/* pointer to last residue */
  Atom *atoms;			/* pointer to atom list */
  struct Structure *structure;	/* pointer to whole structure */
  struct Chain *next;		/* pointer to next chain */
  struct Chain *prev;		/* pointer to previous chain */
} Chain;

typedef struct Structure {
  char *name;			/* name */
  char id[5];			/* id, usually from pdbfile */
  int natoms;			/* number of atoms in asymmetric unit */
  int nhetatoms;		/* number of het atoms in asymmetric unit */
  int nres;			/* number of residues */
  int nchain;			/* number of chains */
  Point3 cg;			/* center-of-gravity */
  float rad;			/* radius from cg that includes all atoms */
  float cell[6];		/* unit cell */
  char spacegroup[12];		/* space group */
  int z;			/* molecules per asymmetric unit */
  struct Atom *atoms;		/* pointer to first atom */
  struct Residue *res;		/* pointer to first residue */
  struct Chain *chain;		/* pointer to first chain */
  struct Structure *next;	/* pointer to next structure record */
  struct index *reslist;
} Structure;

typedef struct Zone {
  Chain *ch1, *ch2;		/* first and last chain   in zone */
  Residue *res1, *res2;		/*   -    -   -   residue  -  -   */
  Atom *atm1, *atm2;		/*   -    -   -   atom     -  -   */
  int nres;			/* number of residues in this zone */
} Zone;

/* prototypes */

Atom *atm_create_atom(void);
Atom *atm_find_atom(Residue *res, const char *atmnam);
Atom *atm_find_named_atom(Structure *st, const char *atomcode);
AtomFile *atm_dopen_file(int fd, char *mode);
AtomFile *atm_open_file(const char *fnam, const char *mode);
Chain *atm_create_chain(void);
Chain *atm_delete_chain(Chain *chain);
Chain *atm_find_named_chain(Structure *st, const char *name);
Chain *atm_find_chain_id(Structure *st, const char chid);
Matrix3 *atm_aniso_to_M3(Atom *atom);
Residue *atm_create_residue(void);
Residue *atm_find_residue(Chain *chain, char *name);
Structure *atm_create_structure(void);
Structure *atm_delete_structure(Structure *s);
Structure *atm_read_pdbfile (AtomFile *f);
Zone *atm_atom_zone(Structure *st, char *from, char *to);
Zone *atm_residue_zone(Structure *st, char *from, char *to);
char *atm_z_to_atom (int z);
int atm_atom_to_z (const char *name);
int atm_decode_atmnam (const char *in, char *chnm, char *resnm, char *atmnm);
int atm_decode_resnam (const char *in, char *chnm, char *resnm);
int atm_read_pdbrecord (char *buf, int siz, AtomFile *f);
void atm_chain_bav(Chain *chain);
void atm_chain_cg(Chain *chain);
void atm_chain_connect(Chain *chain);
void atm_chain_out(Chain *chain);
void atm_chain_what_residue_class(Chain *chain);
void atm_close_file(AtomFile *f);
void atm_residue_class_out(unsigned int residue_class);
void atm_structure_out(Structure *s);
#define _ATOMLIB_H

#ifdef __cplusplus
}
#endif

#endif /* atom.h */

/*
  Local variables:
  mode: font-lock
  End:
*/
