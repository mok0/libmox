/*
 *  $Id$
 *
 *  My PDB decoding routines. 
 *  mk 950309.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pdblib.h"

#define RECSIZ 100
#define EOS '\0'

static char *Elements[] = { "??",
  " H", "He", "Li", "Be", " B", " C", " N", " O" ," F", "Ne",
  "Na", "Mg", "Al", "Si", " P", " S", "Cl", "Ar", " K", "Ca",
  "Sc", "Ti", " V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
  "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", " Y", "Zr",
  "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn",
  "Sb", "Te", " I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",
  "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb",
  "Lu", "Hf", "Ta", " W", "Re", "Os", "Ir", "Pt", "Au", "Hg",
  "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",
  "Pa", " U"
};

/**
  pdb_get_next_atom -- read and decode the next atom record from a PDB file. 
  Return NULL pointer if not found. The routine allocates one pdb atom record.
  mk 950309
*/
pdb_atom_record *
pdb_get_next_atom (FILE *f)
{
  char buf[RECSIZ], *ch;
  int isatom;
  pdb_atom_record *atom;

  isatom = 0;
  while (!isatom && fgets(buf, RECSIZ, f))
    if ((strncmp(buf, "ATOM", 4) == 0) || strncmp(buf,"HETATM",6) == 0)
      isatom = 1;

  if (isatom != 1)
    return NULL;

  if ((atom = (pdb_atom_record *)malloc (sizeof (pdb_atom_record))) == NULL) {
    fprintf (stderr, "could not allocate space for atom record\n");
    return NULL;
  }
  atom->next = atom->prev = NULL;
  atom->serial = atoi (&buf[6]);
  strncpy (atom->name, &buf[12], 4);   atom->name[4] = EOS;
  strncpy (atom->chem, &buf[12], 2);   atom->chem[2] = EOS;
  atom->remote = buf[14];
  atom->branch = buf[15];

#ifdef USEPRIME
  /* pdb uses * for ribose atoms, I use prime ... */
  while ((ch = strstr(atom->name, "*")) != NULL)
    *ch = '\'';
#endif

  atom->Z = pdb_atom_to_z (atom->name);
  strleft (atom->name);
  atom->aloc = buf[16];
  strncpy (atom->restype, &buf[17], 3); atom->restype[3] = EOS;
  strleft(atom->restype);
  strncpy (atom->resname, &buf[21], 6); atom->resname[6] = EOS;
  strleft(atom->resname);
  atom->chain = buf[21];
  atom->resins = buf[26];
  buf[26] = ' ';		/* zap insertion code */
  atom->resno = atoi(&buf[22]);

  if (sscanf (&buf[31], "%lf %lf %lf", &atom->xyz.x, &atom->xyz.y, 
	      &atom->xyz.z) != 3) {
    fprintf (stderr, "could not decode coordinates from record:\n%s\n", buf);
    free(atom);
    return NULL;
  }

  if (sscanf (&buf[54], "%f", &atom->occ) != 1) {
    fprintf (stderr, "could not decode occupancy from record:\n%s\n", buf);
    free(atom);
    return NULL;
  }

  if (sscanf (&buf[60], "%f", &atom->b) != 1) {
    fprintf (stderr, "could not decode b-factor from record: %s\n", buf);
    free(atom);
    return NULL;
  }
  return atom;
}

/**
   pdb_decode_atom_record -- decode an atom record from a PDB file. 
   Return NULL pointer if not found. The routine allocates one pdb atom record.
   mk 950309
*/
pdb_atom_record *
pdb_decode_atom_record (char *buf)
{
  char *ch;
  //int isatom;
  pdb_atom_record *atom;

  if (strncmp (buf,"ATOM",4) != 0)
    return NULL;

  if ((atom = (pdb_atom_record *)malloc (sizeof (pdb_atom_record))) == NULL) {
    fprintf (stderr, "could not allocate space for atom record\n");
    return NULL;
  }
  atom->next = atom->prev = NULL;
  atom->serial = atoi (&buf[6]);
  strncpy (atom->name, &buf[12], 4);   atom->name[4] = EOS;
  strncpy (atom->chem, &buf[12], 2);   atom->chem[2] = EOS;
  atom->remote = buf[14];
  atom->branch = buf[15];

#ifdef USEPRIME
  /* pdb uses * for ribose atoms, I use prime ... */
  while ((ch = strstr(atom->name, "*")) != NULL)
    *ch = '\'';
#endif

  atom->Z = pdb_atom_to_z (atom->name);
  strleft (atom->name);
  atom->aloc = buf[16];
  strncpy (atom->restype, &buf[17], 3); atom->restype[3] = EOS;
  strleft(atom->restype);
  strncpy (atom->resname, &buf[21], 6); atom->resname[6] = EOS;
  strleft(atom->resname);
  atom->resins = buf[26];
  buf[26] = ' ';		/* zap insertion code */
  atom->resno = atoi(&buf[22]);
  atom->chain = buf[21];

  if (sscanf (&buf[31], "%lf %lf %lf", &atom->xyz.x, &atom->xyz.y, 
	      &atom->xyz.z) != 3) {
    fprintf (stderr, "could not decode coordinates from record:\n%s\n", buf);
    free(atom);
    return NULL;
  }

  if (sscanf (&buf[54], "%f", &atom->occ) != 1) {
    fprintf (stderr, "could not decode occupancy from record:\n%s\n", buf);
    free(atom);
    return NULL;
  }

  if (sscanf (&buf[60], "%f", &atom->b) != 1) {
    fprintf (stderr, "could not decode b-factor from record: %s\n", buf);
    free(atom);
    return NULL;
  }
  return atom;
}
  
/*
   pdb_atom_to_z -- given a pdb atom name, decode the element number.
   mk 950309.
*/
int 
pdb_atom_to_z (char *name)
{
  register int i;

  for (i=0; i<92; i++) {
    if (strncasecmp (name, Elements[i], 2) == 0)
      return i;
  }
  fprintf (stderr, "unknown element: %s\n", name);
  return 0;
}

/*
   pdb_z_to_atom -- given an atomic number, return the element name
   mok 950818
*/
char *
pdb_z_to_atom (int z)
{
  z = ( z > 92 ? z : 92);
  z = ( z < 0 ? z : 0);
  return Elements[z];
}


/*
   pdb_read_atoms -- read in the atom datastructure from a pdb file.
   Returns a pointer to the first atom record. The record following the last
   atom or hetatm in the file is eaten.
   mk 950309
*/
pdb_atom_record *
pdb_read_atoms (FILE *f)
{

  pdb_atom_record *first, *atom;
  int was, irc;
  char wasins;

  first = atom = pdb_get_next_atom(f);
  while (atom) {
    atom->next = pdb_get_next_atom(f);
    atom = atom->next;
  }

  /* count residues and generate irc */
  
  atom = first;
  irc = 0;
  was = -9999;
  wasins = 0;
  while (atom) {
    if (atom->resno != was || atom->resins != wasins) {
      irc++;
      atom->irc = irc;
      was = atom->resno;
      wasins = atom->resins;
    }
    atom = atom->next;
  }

  return first;
}

/*
   pdb_atoms_out -- dump the datastructure to stdout.
*/
void 
pdb_atoms_out (pdb_atom_record *atom)
{
  while (atom) {
    printf("ATOM  %5d %2s%c%c%c%-3s %c%4d%c   %8.3f%8.3f%8.3f%6.2f%6.2f %3d\n",
	   atom->serial, atom->chem, atom->remote, atom->branch, atom->aloc, 
	   atom->restype, atom->chain, atom->resno, atom->resins, 
	   atom->xyz.x, atom->xyz.y, atom->xyz.z, atom->occ, atom->b, atom->Z);
    atom = atom->next;
  }
}

/** 
   pdb_get_cryst_record -- search for the next CRYST1 record, decode it, and
   return a pointer to a pdb_cryst_record datastructure. The record after the
   cryst record is eaten by this routine.
   mk 950309
*/
pdb_cryst_record *
pdb_get_cryst_record (FILE *f)
{
  char buf[RECSIZ];
  int iscryst;

  iscryst = 0;
  while (!iscryst && fgets(buf,RECSIZ,f)) {
    if (strncmp(buf, "CRYST", 5) == 0)
      iscryst = 1;
    if (strncmp(buf, "ATOM", 4) == 0)
      return NULL;
  }

  if (iscryst != 1)		/* this means end of file! */
    return NULL;

  return pdb_decode_cryst_record (buf);
}


/**
   pdb_decode_cryst_record -- decode the CRYST1 record from a PDB file and
   return a pointer to a pdb_cryst_record datastructure.
   mk 950405.
*/
pdb_cryst_record *
pdb_decode_cryst_record (char *buf)
{
  pdb_cryst_record *cryst;

  if (strncmp(buf, "CRYST", 5) != 0) {
    return NULL;
  }

  if ((cryst = (pdb_cryst_record *)malloc(sizeof (pdb_cryst_record))) == NULL) {
    fprintf (stderr, "could not allocate space for cryst record\n");
    return NULL;
  }
  strncpy (cryst->spacegroup, &buf[55], 11);   cryst->spacegroup[11] = EOS;
  strleft (cryst->spacegroup);

  if (sscanf (&buf[6], "%lf %lf %lf %lf %lf %lf",
	   &cryst->cell[0], &cryst->cell[1], &cryst->cell[2],
	   &cryst->cell[3], &cryst->cell[4], &cryst->cell[5]) != 6) {
    fprintf (stderr, "could not decode unit cell from record:\n%s\n", buf);
    free(cryst);
    return NULL;
  }
  cryst->z = atoi(&buf[66]);

  return cryst;
}

/**
   pdb_decode_cmpnd_record -- decode the CMPND record from a PDB file and
   return a pointer to a pdb_cmpnd_record datastructure.
   mk 950404.
*/
pdb_compnd_record *
pdb_decode_compnd_record (char *buf)
{
  pdb_compnd_record *compnd;
  extern char *strtrim();

  if ((compnd = (pdb_compnd_record *)malloc(sizeof (pdb_compnd_record))) == NULL) {
    fprintf (stderr, "could not allocate space for compnd record\n");
    return NULL;
  }
  
  buf[70] = EOS;
  compnd->text = (char *)malloc(strlen(strtrim(&buf[10])));
  strcpy (compnd->text,&buf[10]);
  return compnd;
}

/**
   pdb_read_file -- read an entire pdb file, and decode some of the interesting 
   records. The file stream f must be open.
   mk 950404.
*/
PDBfile *
pdb_read_file (FILE *f)
{
  PDBfile *p;
  pdb_atom_record *atom;
  pdb_compnd_record *compound;
  char buf[100];
  int done = 0;
  int was;
  char wasins;
  
  compound = NULL;

  if ((p = (PDBfile *)malloc(sizeof (PDBfile))) == NULL) {
    fprintf (stderr, "could not allocate space for PDBfile record\n");
    return NULL;
  }

  p->f = f;
  p->id[0] = '\0';
 
  while (fgets(buf, 100, f) && ! done) {

    if (strncmp(buf,"ATOM",4) == 0 || strncmp (buf,"HETA",4) == 0) {
      p->ATOM = atom = pdb_decode_atom_record (buf);
      p->natoms = 0;
      while (atom) {
	p->natoms++;
	atom->next = pdb_get_next_atom(f);
	atom = atom->next;
      }

    } else if (strncmp (buf, "HEADER", 6) == 0) {
      strncpy (p->id, &buf[62], 4);
      p->id[4] = '\0';
 
    } else if (strncmp (buf, "CRYS", 4) == 0) {
      p->CRYST1 = pdb_decode_cryst_record (buf);

    }  else if (strncmp (buf, "COMP", 4) == 0) {
      if (compound == NULL)
	p->COMPND = compound = pdb_decode_compnd_record (buf);
      else {
	compound->next = pdb_decode_compnd_record(buf);
	compound = compound->next;
      }

    } else if (strncmp (buf, "END", 3) == 0) {
      printf ("end"); done = 1;
    } 
  }

  /* count residues and generate irc */
  
  atom = p->ATOM;
  p->nres = 0;
  was = -9999;
  wasins = 0;
  while (atom) {
    if (atom->resno != was || atom->resins != wasins) {
      p->nres++;
      atom->irc = p->nres;
      was = atom->resno;
      wasins = atom->resins;
    }
    atom = atom->next;
  }
  return p;
}


/**
   pdb_decode_helix_record -- decode the HELIX record from a PDB file and
   return a pointer to a pdb_ss_record datastructure.
   mk 950404.
*/

pdb_ss_record *
pdb_decode_helix_record (char *buf)
{
  pdb_ss_record *helix;
  extern char *strtrim();

  if ((helix = (pdb_ss_record *)malloc(sizeof (pdb_ss_record))) == NULL) {
    fprintf (stderr, "could not allocate space for helix record\n");
    return NULL;
  }
  helix->what = 1;
  helix->serial = atoi(&buf[7]);
  strncpy (helix->name, &buf[11], 3);   helix->name[3] = 0;
  strncpy (helix->restype1, &buf[15], 3);   helix->restype1[3] = EOS;
  strncpy (helix->restype2, &buf[27], 3);   helix->restype2[3] = EOS;

  helix->from[0] = buf[19];
  strncpy (&helix->from[1], &buf[21],4);
  helix->from[5] = buf[25];
  helix->from[6] = EOS;
  strleft (helix->from);

  helix->to[0] = buf[31];
  strncpy (&helix->to[1], &buf[33],4);
  helix->to[5] = buf[37];
  helix->to[6] = EOS;
  strleft (helix->to);

  helix->class = atoi(&buf[38]);
  helix->next = NULL;
  return helix;
}


/**
   pdb_decode_sheet_record -- decode the SHEET record from a PDB file and
   return a pointer to a pdb_ss_record datastructure.
   mk 950404.
*/

pdb_ss_record *
pdb_decode_sheet_record (char *buf)
{
  pdb_ss_record *sheet;
  extern char *strtrim();

  if ((sheet = (pdb_ss_record *)malloc(sizeof (pdb_ss_record))) == NULL) {
    fprintf (stderr, "could not allocate space for sheet record\n");
    return NULL;
  }

  sheet->what = 2;
  sheet->serial = atoi(&buf[7]);
  strncpy (sheet->name, &buf[11], 3);       sheet->name[3] = EOS;
  strncpy (sheet->restype1, &buf[17], 3);   sheet->restype1[3] = EOS;
  strncpy (sheet->restype2, &buf[28], 3);   sheet->restype2[3] = EOS;

  sheet->from[0] = buf[21];
  strncpy (&sheet->from[1], &buf[22],4);
  sheet->from[5] = buf[26];
  sheet->from[6] = EOS;
  strleft (sheet->from);

  sheet->to[0] = buf[32];
  strncpy (&sheet->to[1], &buf[33],4);
  sheet->to[5] = buf[37];
  sheet->to[6] = EOS;
  strleft (sheet->to);

  sheet->next = NULL;
  return sheet;
}


/* 
   Local Variables:
   mode: font-lock
   End:
*/

