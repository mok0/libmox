/** @file

   $Id$

   My atom library, version 3. The most significant changes from version 2 
   is in function atm_read_pdbfile, which now decodes a pdb file directly 
   into the datastructure.
   mok, January 1997
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_LIBZ
#  include <zlib.h>
#endif
#include "atom.h"
#include "pdb.h"

#define SEP1 ':'
#define SEP2 '.'

/**
   Decode a residue name of the form a:123. Chain
   name 'a' is returned in 'chnm', residue name in 'resnm'. The strings
   chnm and resnm must be large enough, no checks made.
*/

int atm_decode_resnam (char *in, char *chnm, char *resnm)
{
  char *c;
  int i;

  if (!in) {
    *chnm = 0;
    *resnm = 0;
    return 0;
  }

  c = strchr(in, ':');

  if (c == NULL) {		
    *chnm = 0;
    strcpy (resnm, in);
    return 1;
  } 
  
  i = c-in;
  strncpy (chnm, in, i);
  chnm[i] = 0;

  c = strrchr (in, ':');
  i = c-in;
  strcpy (resnm, &in[i+1]);
  return 1;
}


/**
   Decode an atom name of the form a:123.ca . Chain name 'a' is returned in
   'chnm', residue name in 'resnm', atom name in 'atnm'. This routine should
   handle the most degenerate cases.
*/

int atm_decode_atmnam (char *in, char *chnm, char *resnm, char *atmnm)
{
  char *c1, *c2;
  int i, j;

  if (!in) {
    *chnm = 0;
    *resnm = 0;
    *atmnm = 0;
    return 0;
  }

  c1 = strchr(in, SEP1);
  c2 = strrchr(in, SEP2);

  if (c2 == NULL && c1 == NULL) { /* ca is atmname only */
    *chnm = 0;
    *resnm = 0;  
    strcpy (atmnm, in);
    return 1;
  }
  
  if (c1 == NULL) {		/* handle 93.ca and .ca and . */
    *chnm = 0;
    i = c2-in;
    strncpy (resnm, in, i);	/* everything before . is resnm */
    resnm[i] = 0;
    strcpy (atmnm, &in[i+1]);
    return 1;
  }

  if (c2 == NULL) {		/* : was seen but not . */
    *atmnm = 0;
    atm_decode_resnam(in, chnm, resnm);
    return 1;
  }

  /* if we get to there, both SEP1 and SEP2 were seen */

  i = c1-in;
  strncpy (chnm, in, i);
  chnm[i] = 0;

  j = c2-in;
  strcpy(atmnm, &in[j+1]);

  strncpy(resnm, &in[i+1], j-i-1); 
  resnm[j-i-1] = 0;

  return 1;
}

/**
  open an atom file -- this version handles compressed files too.
*/
AtomFile *atm_open_file(char *fnam, char *mode)
{
  AtomFile *f;

  f = (AtomFile *)malloc (sizeof (AtomFile));

  f->name = strdup (fnam);
  f->mode = strdup (mode);
#ifdef HAVE_LIBZ
  f->File = gzopen (fnam, mode);
#else
  f->File = fopen (fnam, mode);
#endif
 
  if (!f->File)
    return NULL;
  else
    return f;
}

/**
  open an atom file and associate file with a file descriptor. This
  version handles compressed files too.
*/

AtomFile *atm_dopen_file(int fd, char *mode)
{
  AtomFile *f;

  f = (AtomFile *)malloc (sizeof (AtomFile));

  f->name = NULL;
  f->mode = strdup (mode);
#ifdef HAVE_LIBZ
  f->File = gzdopen (fd, mode);
#else
  f->File = fdopen (fd, mode);
#endif
 
  if (!f->File)
    return NULL;
  else
    return f;
}

/**
   Close a file opened by atm_open_file.
*/

void atm_close_file (AtomFile *f)
{
  if (!f)
    return;

  if (f->File) {
#ifdef HAVE_LIBZ
    gzclose (f->File);
#else
    fclose(f->File);
#endif
  }
  free(f);
  return;
}

/**
   read a pdb file, and return a pointer to a new
   Structure record. Pass a file pointer to an open file.
*/

Structure *atm_read_pdbfile (AtomFile *f)
{
  Structure *s;
  Atom *theatom, *newatom;
  Residue *theresidue, *newresidue;
  Chain *thechain, *newchain, *lastchain;
  pdb_atom_record pdbatom;
  char buf[100];
  int what, done, natoms, nhetatoms, hetflag;
  char chain_was, residue_ins_was;
  int residue_was, isfirstchain, isfirstresidue, isfirstatom;
  int ctatom, ctres;

  if (!f)
    return NULL;

  isfirstchain = isfirstresidue = isfirstatom = TRUE;
  done = 0;
  hetflag = FALSE;
  natoms = nhetatoms = 0;
  chain_was = '?';
  residue_was = -999;
  ctres = ctatom = 0;

  s = atm_create_structure();

  what = atm_read_pdbrecord (buf,100,f);
  while (what > -1 && !done) {
    switch (what) {

    case HEADER:
      strncpy (s->id, &buf[62], 4);
      s->id[4] = '\0';
      strlower(s->id);
      s->name = s->id;
      break;

    case REMARK:
      break;

    case COMPND:
      break;

    case CRYST1:
      strncpy (s->spacegroup, &buf[55], 11);   
      s->spacegroup[11] = 0;
      strleft (s->spacegroup);
      if (sscanf (&buf[6], "%f %f %f %f %f %f",
		  &s->cell[0], &s->cell[1], &s->cell[2],
		  &s->cell[3], &s->cell[4], &s->cell[5]) != 6) {
	fprintf (stderr, "could not decode unit cell from record:\n%s\n", buf);
      }
      s->z = atoi(&buf[66]);
      break;

    case HETATM:
      hetflag = TRUE;
      nhetatoms++;
    case ATOM:

      if (!decode_pdb_atom (&pdbatom, buf))
	break;

      /* check to see if this is the beginning of a new chain.
	 if so, make a new chain and a new residue */

      if (pdbatom.chain != chain_was) {	

	if (!(newchain = atm_find_chain_id(s, pdbatom.chain))) {

	  chain_was = pdbatom.chain;
	  newchain = atm_create_chain();

	  if (isfirstchain) {
	    /* hook chain into the Structure */
	    s->chain = newchain;
	    lastchain = newchain;
	    isfirstchain = FALSE;
	  } else {
	    /* hook new chain into list */
	    lastchain->next = newchain;
	    newchain->prev = lastchain;
	    lastchain->last = theresidue;
	    lastchain = newchain;
	  }

	  ++(s->nchain);
	  //fprintf (stderr, "New chain %i (%c)\n", s->nchain, pdbatom.chain);
	  thechain = newchain;
	  thechain->id = pdbatom.chain;
	  thechain->name[0] = thechain->id;
	  thechain->structure = s;

	  /* we are beginning a new chain, make a new residue */
	  residue_was = -999;
	  isfirstresidue = TRUE;
	  ctres = 0;
	  // ctatom continues
	
	} else {

	  // add to existing chain

	  //fprintf (stderr, "Adding to existing chain (%c)\n",pdbatom.chain);
	  chain_was = pdbatom.chain;
	  isfirstresidue = FALSE;

	  thechain = newchain;
	  thechain->id = pdbatom.chain;
	  thechain->name[0] = thechain->id;
	  thechain->structure = s;

	  theresidue = newchain->last;

	  residue_was = -999;  // force creation of new residue
	  isfirstresidue = FALSE;
	  ctres = thechain->nres;

	}
      }
      
      /* check to see if this is a new residue. If so, make one */

      if (residue_was != pdbatom.resno || residue_ins_was != pdbatom.resins) {
	residue_ins_was = pdbatom.resins;
	residue_was = pdbatom.resno;
	newresidue = atm_create_residue();

	/* if this is the first residue in a chain, its backward pointer 
	   is NULL. If not, hook it into the list of residues */

	if (isfirstresidue) {
	  newresidue->prev = NULL;
	  isfirstresidue = FALSE;
	  thechain->res = newresidue;
	  thechain->first = newresidue;
	} else {
	  theresidue->next = newresidue;
	  newresidue->prev = theresidue;
	}

	++(s->nres);
	++(thechain->nres);
	theresidue = newresidue; 
	thechain->last = theresidue;  // this is the last residue so far
	strcpy (theresidue->name, pdbatom.resname);
	strcpy (theresidue->type, pdbatom.restype);
	theresidue->resno = pdbatom.resno;
	theresidue->resins = pdbatom.resins;
	if (theresidue->resins > 32)
	  ++(thechain->nresins);
	theresidue->ic = ++ctres;
	theresidue->chain = thechain;
	isfirstatom = TRUE;
      }

      /* create a new atom, and hook it into the datastructure */

      newatom = atm_create_atom();

      /* if this is the first atom in a residue, it's backward pointer
	 is NULL, otherwise hook it into the bi-linked list of atoms */

      if (isfirstatom) {
	newatom->prev = NULL;
	isfirstatom = FALSE;
	theresidue->atoms = newatom;
      } else {
	theatom->next = newatom;
	newatom->prev = theatom;
      }

      ++(s->natoms);
      ++(thechain->natoms);
      ++(theresidue->natoms);
      theatom = newatom;
      strcpy (theatom->name, pdbatom.name);
      theatom->ins = pdbatom.aloc;
      if (theatom->ins > 32) {
	++(theresidue->naltatm);
	++(thechain->naltatm);
      }
      theatom->z = pdbatom.z;
      theatom->xyz = pdbatom.xyz;
      theatom->b = pdbatom.b;
      theatom->occ = pdbatom.occ;
      theatom->ic = ++ctatom;
#ifdef DEBUG
      if (theatom->ic != pdbatom.serial)
	fprintf (stderr, "error in serial number: %d, %d from file\n",
		 theatom->ic, pdbatom.serial);
#endif
      theatom->res = theresidue;
      if (hetflag) {
	setbit (theatom->flag, 30); /* set hetatm flag */
	++(thechain->nhetatoms);
	hetflag = FALSE;
      }
      break;

    case ANISOU:
      {
	pdb_aniso_record pdbaniso;
	register i;

	if (!decode_pdb_aniso (&pdbaniso, buf))
	  break;

	if (pdbaniso.serial != theatom->ic) {
	  fprintf (stderr,"warning, aniso record does not match atom [%d, %d]\n", pdbaniso.serial, theatom->ic);
	  break;
	}

	theatom->aniso = (float *)calloc(6, sizeof(float));
	for (i=0; i<6; i++)
	  theatom->aniso[i] = pdbaniso.aniso[i]/10000.0;

#ifdef DEBUG
	{
	  Matrix3 *m;

	  m = atm_aniso_to_M3(theatom);
	  M3Print("Aniso matrix", m);
	  free(m);
	}
#endif
      }

      break;

    case HELIX:

    case SHEET:
      break;

    case TER:
      //chain_was = '?'; // force new chain
      ++ctatom;
      break;

    case ENDFILE:
      done = 1;
    }

    if (!done)
      what = atm_read_pdbrecord (buf,100,f);
  }

  /* file is read -- now fill out the bits and pieces we didn't know 
     about before... */

  thechain = s->chain;
  while (thechain) {

    /* calculate the center-of-gravity of every residue */
    atm_chain_cg(thechain); 

    /* calculate the average B-factor of every residue */
    atm_chain_bav(thechain); 

    /* what can we find out about the class of residue and what atoms
       it contains? */
    atm_chain_what_residue_class(thechain);

    /* figure something out about the connectivity of this chain */
    atm_chain_connect(thechain);

    thechain = thechain->next;
  }

  return s;
}

/**
   read one record from an open file, determine its
   type and return it.
*/
int atm_read_pdbrecord (char *buf, int siz, AtomFile *f)
{
#ifdef HAVE_LIBZ
  if (!gzgets (f->File, buf, siz)) 
    return -1;
#else
  if (!fgets(buf, siz, f->File))
    return -1;
#endif

  if (strncmp(buf,"ATOM", 4) == 0)
    return ATOM;
  else if (strncmp(buf, "REMARK", 7) == 0)
    return REMARK;
  else if (strncmp(buf, "HEADER", 6) == 0)
    return HEADER;
  else if (strncmp(buf, "COMPND", 6) == 0)
    return COMPND;
  else if (strncmp(buf, "CRYST1", 6) == 0)
    return CRYST1;
  else if (strncmp(buf, "HELIX", 5) == 0)
    return HELIX;
  else if (strncmp(buf, "SHEET", 5) == 0)
    return SHEET;
  else if (strncmp(buf, "END", 3) == 0)
    return ENDFILE;
  else if (strncmp(buf, "HETATM", 6) == 0)
    return HETATM;
  else if (strncmp(buf, "ANISOU", 6) == 0)
    return ANISOU;
  else if (strncmp(buf, "TER", 3) == 0)
    return TER;
  else
    return 999;
}

/**
   Decode the string describing one ATOM record from 
   a pdb file and fill in some of the fields.
*/

static pdb_atom_record *decode_pdb_atom (pdb_atom_record *atom, char *buf)
{

  decode_pdb_atom_id (atom, buf);

  char xbuf[10], ybuf[10], zbuf[10];
  char occbuf[10], bbuf[10];
  char *endp;

  strncpy(xbuf, &buf[30], 8);
  xbuf[8] = 0;
  strncpy(ybuf, &buf[38], 8);
  ybuf[8] = 0;
  strncpy(zbuf, &buf[46], 8);
  zbuf[8] = 0;

  atom->xyz.x = strtod(xbuf, &endp);
  atom->xyz.y = strtod(ybuf, &endp);
  atom->xyz.z = strtod(zbuf, &endp);
  
  /*
  if (sscanf (&buf[30], "%lf %lf %lf", &atom->xyz.x, &atom->xyz.y, 
	      &atom->xyz.z) != 3) {
    fprintf (stderr, "could not decode coordinates from record:\n%s\n", buf);
    free(atom);
    return NULL;
  }
  */

  strncpy(occbuf, &buf[54], 6);
  occbuf[6] = 0;
  atom->occ = strtod(occbuf, &endp);

  /*
  if (sscanf (&buf[54], "%f", &atom->occ) != 1) {
    fprintf (stderr, "could not decode occupancy from record:\n%s\n", buf);
    free(atom);
    return NULL;
  }
  */

  strncpy(bbuf, &buf[60], 6);
  bbuf[6] = 0;
  atom->b = strtod(bbuf, &endp);
  /*
  if (sscanf (&buf[60], "%f", &atom->b) != 1) {
    fprintf (stderr, "could not decode b-factor from record: %s\n", buf);
    free(atom);
    return NULL;
  }
  */
  return atom;
}

/**
   Decode the string describing one ANISO record from 
   a pdb file and fill in some of the fields.
*/


static pdb_aniso_record *decode_pdb_aniso (pdb_aniso_record *aniso, char *buf)
{

  if (sscanf (&buf[31], "%f %f %f %f %f %f", 
	      &aniso->aniso[0], &aniso->aniso[1], &aniso->aniso[2], 
	      &aniso->aniso[3], &aniso->aniso[4], &aniso->aniso[5]) != 6) {
    fprintf (stderr, 
	     "could not decode anisotropic matrix from record:\n%s\n", buf);
    return NULL;
  }

  aniso->serial = atoi (&buf[6]);

  return aniso;
}


/**
   Decode the part of the string describing one 
   ATOM record from a pdb file and fill in some of the fields concerning
   atom and residue id (the parts that are identical in an ATOM and ANISO 
   record)
*/

static void decode_pdb_atom_id (pdb_atom_record *atom, char *buf)
{
  char *ch;

  atom->serial = atoi (&buf[6]);
  strncpy (atom->name, &buf[12], 4);   atom->name[4] = 0;
  strncpy (atom->chem, &buf[12], 2);   atom->chem[2] = 0;
  atom->remote = buf[14];
  atom->branch = buf[15];

#ifdef USEPRIME
  /* pdb uses * for ribose atoms, I use prime ... */
  while ((ch = strstr(atom->name, "*")) != NULL)
    *ch = '\'';
#endif

  atom->z = atm_atom_to_z (atom->name);
  strleft (atom->name);
  atom->aloc = buf[16];
  strncpy (atom->restype, &buf[17], 3); atom->restype[3] = 0;
  strtrim(strleft(atom->restype));
  strncpy (atom->resname, &buf[22], 5); atom->resname[5] = 0; 
  strtrim(strleft(atom->resname));
  atom->resins = buf[26];
  buf[26] = ' ';		/* zap insertion code */
  atom->resno = atoi(&buf[22]);
  atom->chain = buf[21];
  if (atom->chain < 33)
    atom->chain = '_';

  return;
}



/**
   Step through residues, see if we can find out what type of residue
   it is.
*/
void atm_chain_what_residue_class (Chain *chain)
{
  Residue *res;
  Atom *theatom;

  unsigned long flag, proteinflag = 0, dnaflag = 0, rnaflag = 0;

  setbit(proteinflag, ATOM_CA);
  setbit(proteinflag, ATOM_N);
  setbit(proteinflag, ATOM_O);
  setbit(proteinflag, ATOM_C);

  setbit(dnaflag, ATOM_P);
  setbit(dnaflag, ATOM_O2P);

  setbit(rnaflag, ATOM_P);
  setbit(rnaflag, ATOM_O2P);
  setbit(rnaflag, ATOM_O3P);

  res = chain->res;

  while (res) {
    int pcount = 0;		/* number of phosphates in this residue */
    flag = 0;
    theatom = res->atoms;
    while (theatom) {

      if (strcmp(theatom->name, "CA") == 0 && theatom->z == 6) { 
	setbit(flag, ATOM_CA);	/* 1 */
	res->ca = theatom;	/* set central atom */
      }

      else if (strcmp(theatom->name, "N") == 0) {
	setbit(flag, ATOM_N);	/* 2 */
      }

      else if (strcmp(theatom->name, "O") == 0) {
	setbit(flag, ATOM_O);	/* 4 */
      }

      else if (strcmp(theatom->name, "C") == 0) {
	setbit(flag, ATOM_C);	/* 8 */
      }

      else if (theatom->z  == 15) {
	++pcount;
	setbit(flag, ATOM_P);	/* 16 */
	res->ca = theatom;	/* set central atom */
      }

#ifdef USEPRIME
      else if (strcmp(theatom->name, "O2'") == 0) {
	setbit(flag, ATOM_O2P);	/* 32 */
      }

      else if (strcmp(theatom->name, "O3'") == 0) {
	setbit(flag, ATOM_O3P);	/* 32 */
      }
#else
      else if (strcmp(theatom->name, "O2*") == 0) {
	setbit(flag, ATOM_O2P);	/* 32 */
      }

      else if (strcmp(theatom->name, "O3*") == 0) {
	setbit(flag, ATOM_O3P);	/* 32 */
      }
#endif
      else if (strcmp(theatom->name, "N9") == 0) {
	setbit(res->res_class, PURINE);
      }

      theatom = theatom->next;
    }

    if (pcount > 1) {
      /* assumption: if we have more than one phosphate,
	 it is a nucleotide */
      setbit(res->res_class, POLYPHOSPHATE);
    }

    res->flag = flag;

    /* single oxygen atoms are water molecules */
    if (res->natoms == 1 && res->atoms->z == 8) {
      setbit (res->res_class, SOLVENT);
      setbit(chain->chain_class, SOLVENT);
    }

    if (flag == proteinflag) {
      setbit(chain->chain_class, PROTEIN); 
      setbit(res->res_class, AMINOACID);
    }

    if (flag >= dnaflag) {
	setbit(chain->chain_class, NUCLEIC_ACID);
	if (flag == dnaflag) {
	  setbit(chain->chain_class, DNA);
	  setbit(res->res_class, NUCLEOTIDE);
	  setbit(res->res_class, DNABASE);
	}
	if (flag == rnaflag) {
	  setbit(chain->chain_class, RNA);
	  setbit(res->res_class, NUCLEOTIDE);
	  setbit(res->res_class, RNABASE);
	}
	if (!isset(res->res_class, PURINE))
	  setbit(res->res_class, PYRIMIDINE);
    }

    /* count non-water HET groups */
    if (isclear(res->res_class,AMINOACID) &&
	isclear(res->res_class,SOLVENT)) {
      ++(chain->hetcount);
    }
    


    res = res->next;
  }
}

/**
   Figure something out about the connectivity of the
   residues in the chain. (At the moment, this routine does not do any
   real checks.)
*/

void 
atm_chain_connect (Chain *chain)
{
  Residue *res;

  res = chain->res;

  while (res) {
    if (1)			/* dummy for now */
      res->bond_next = res->next;
    if (1)
      res->bond_prev = res->prev;
    res = res->next;
  }
}

/**
   find a named atom in the specified residue.
*/
Atom *atm_find_atom (Residue *res, char *atmnam)
{
  Atom *theatom;
  char name[8];

  if (res == NULL || atmnam == 0 || (*atmnam == 0))
    return NULL;

  strcpy (name, atmnam);
  
  if (name[0] == '+') {		/* this atom is in the next residue */
    if (res->bond_next) {
      name[0] = ' ';
      strleft(name);
      res = res->bond_next;
    } else {
      return NULL;
    }
    return atm_find_atom (res,name);

  } else if (name[0] == '-') {	/* this atom is in the previous residue */
    if (res->bond_prev) {
      name[0] = ' ';
      strleft(name);
      res = res->bond_prev;
    } else {
      return NULL;
    }
    return atm_find_atom (res,name);
  }

  theatom = res->atoms;
  while (theatom) {
    if (strcasecmp(theatom->name, name) == 0)
      return theatom;
    theatom = theatom->next;
  }
  return NULL;
}

/**
   Find a named residue in the specified chain.
   Rewrite this sometime to use a hashed search.
*/
Residue *atm_find_residue (Chain *chain, char *name)
{
  Residue *theres;

  if (chain == NULL)
    return NULL;

  theres = chain->res;
  while (theres) {
    if (strcasecmp(theres->name, name) == 0)
      return theres;
    theres = theres->next;
  }
  return NULL;
}

/**
   Find a named chain in the specified structure
   Rewrite this sometime to use a hashed search.
*/
Chain *atm_find_named_chain (Structure *st, char *name)
{
  Chain *thechain;

  if (st == NULL)
    return NULL;

  thechain = st->chain;
  while (thechain) {
    if (strcasecmp(thechain->name, name) == 0)
      return thechain;
    thechain = thechain->next;
  }
  return NULL;
}

/**
   Search the structure for a chain of the given identity.
*/
Chain *atm_find_chain_id (Structure *s, char chid)
{
  Chain *chain;

  chain = s->chain;
  while (chain) {
    if (chain->id == chid) {
      return chain;
    }
    chain = chain->next;
  }
  return NULL;
}


/* ==== Routines to output information ==== */

/**
  Output the version of libatom on stderr.
*/
void atm_version_out(void)
{
  char buf[] = "$Revision$";
  char date[] = "$Date$";
  int n;

  n = strlen(buf);  if (n > 2) buf[n-2] = '\0';
  n = strlen(date); if (n > 16) date[17] = '\0';

  fprintf (stderr, "libatom version: %s, %s\n", &buf[11], &date[7]);
}

/**
   Given a pointer to a structure record, output what we
   know about it.
*/
void atm_structure_out (Structure *s)
{
  register i;

  if (s) {
    printf ("structure name: %s, id: %s\n", s->name, s->id);
    printf ("contains %d chains, %d residues and %d atoms\n", 
	     s->nchain, s->nres, s->natoms);
    printf ("cell parameters: ");
    for (i=0; i<6; i++)
      printf ("%8.2f ", s->cell[i]);
    printf ("\nspacegroup: %s, %d molecules in unit cell\n", 
	     s->spacegroup, s->z);
  }
}


/** 
   Output the classes from the input type.
*/
void atm_residue_class_out (unsigned int residue_class)
{
  if (isset(residue_class, PROTEIN))
    printf ("protein ");
  if (isset(residue_class, NUCLEIC_ACID))
    printf ("nucleic acid ");
  if (isset(residue_class, DNA))
    printf ("DNA ");
  if (isset(residue_class, RNA))
    printf ("RNA ");
  if (isset(residue_class, NUCLEOTIDE))
    printf ("nucleotide ");
  if (isset(residue_class, PURINE))
    printf ("purine "); 
  if (isset(residue_class, PYRIMIDINE))
    printf ("pyrimidine ");
  if (isset(residue_class, AMINOACID))
    printf ("amino acid "); 
  if (isset(residue_class, SOLVENT))
    printf ("solvent "); 
}

/** 
   Output the classes from the input type.
*/
void atm_chain_class_out (unsigned int chain_class)
{
  if (isset(chain_class, PROTEIN))
    printf ("protein ");
  if (isset(chain_class, NUCLEOTIDE))
    printf ("(contains nucleotide) ");
  if (isset(chain_class, NUCLEIC_ACID))
    printf ("nucleic acid ");
  if (isset(chain_class, DNA))
    printf ("DNA ");
  if (isset(chain_class, RNA))
    printf ("RNA ");
  if (isset(chain_class, SOLVENT))
    printf ("solvent "); 
}

/**
   Print out a chain list, given the top pointer.
*/
void atm_chain_out (Chain *chain)
{
  while (chain) {
    printf ("chain name: %s chain id: %c residues: %4d atoms: %5d bav: %5.1f",
	    chain->name, chain->id, chain->nres, chain->natoms, chain->bav);
    printf (" zone: %c:%s-%c:%s class: ",
	    chain->id, chain->first->name, chain->id, chain->last->name);
    atm_chain_class_out(chain->chain_class);
    printf ("\n");
    chain = chain->next;
  }
}

/* ==== Miscellaneous computations on chains ==== */

/**
   Calculate the center-of-gravity of this chain and of every residue in the
   chain.
*/
void atm_chain_cg(Chain *chain)
{
  Residue *res;
  Atom *atom;

  while (chain){
    chain->cg.x = chain->cg.y = chain->cg.z = 0.0;
    res = chain->res;
    while (res) {
      res->cg.x = res->cg.y = res->cg.z = 0.0;
      atom = res->atoms;
      while (atom) {
	V3Add (&atom->xyz, &res->cg, &res->cg);
	atom = atom->next;
      }
      if (res->natoms > 1)
	V3Div (&res->cg, (double)res->natoms, &res->cg);
      V3Add (&res->cg, &chain->cg, &chain->cg); 
      res = res->next;
    }
    V3Div (&chain->cg, (double)chain->nres, &chain->cg);
    chain = chain->next;
  }
}

/**
   Calculate the average B factor of this chain and of every residue in the
   chain.
*/
void atm_chain_bav(Chain *chain)
{
  Residue *res;
  Atom *atom;

  while (chain){
    chain->bav = 0.0;
    res = chain->res;
    while (res) {
      res->bav = 0.0;
      atom = res->atoms;
      while (atom) {
	res->bav += atom->b;
	atom = atom->next;
      }
      if (res->natoms > 1)
	res->bav /= res->natoms;
      chain->bav += res->bav;
      res = res->next;
    }
    if (chain->nres > 1)
      chain->bav /= chain->nres;
    chain = chain->next;
  }
}

/* ==== Routines to create datastructures ==== */

/**
   Allocate a new Structure datastructure,
   and fill with zeroes.
 */
Structure *atm_create_structure(void)
{
  Structure *s;

  s = NEWTYPE(Structure); /* allocate a new datastructure */
  if (!s) {
    fprintf (stderr, "memory exhausted\n");
    exit(2);
  }
  memset (s, 0, sizeof(Structure)); /* fill structure with zeros */
  return s;
}

/*
   atm_create_chain -- allocate a new Chain datastructure, 
   and fill with zeroes.
   mok 960726
*/
Chain *atm_create_chain(void)
{
  Chain *s;

  s = NEWTYPE(Chain);
  if (!s) {
    fprintf (stderr, "memory exhausted\n");
    exit(2);
  }
  memset (s, 0, sizeof(Chain)); /* fill structure with zeroes */
  return s;
}

/**
   Allocate a new Residue datastructure, and fill with zeroes.
*/
Residue *atm_create_residue(void)
{
  Residue *s;

  s = NEWTYPE(Residue);
  if (!s) {
    fprintf (stderr, "memory exhausted\n");
    exit(2);
  }
  memset (s, 0, sizeof(Residue)); /* fill structure with zeroes */
  return s;
}

/**
   Allocate a new Atom datastructure, and fill with zeroes.
*/
Atom *atm_create_atom(void)
{
  Atom *s;

  s = NEWTYPE(Atom);		/* allocate a new datastructure */
  if (!s) {
    fprintf (stderr, "memory exhausted\n");
    exit(2);
  }
  memset (s, 0, sizeof(Atom));	/* fill structure with zeroes */
  return s;
}

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
  Given a pdb atom name, decode the element number.
*/
int atm_atom_to_z (char *name)
{
  register int i;

  for (i=0; i<92; i++) {
    if (strncasecmp (name, Elements[i], 2) == 0)
      return i;
  }
  /* fprintf (stderr, "unknown element: %s\n", name); */
  return 0;
}


/**
   Given an atomic number, return the element name.
*/
char *atm_z_to_atom (int z)
{
  z = ( z > 92 ? 92 : z);
  z = ( z < 0 ? 0 : z);
  return Elements[z];
}

/**
   Pass atom paths of the beginning and end of a zone,
   and return a Zone datastructure containing the pointers.
*/
Zone *atm_atom_zone (Structure *st, char *from, char *to)
{
  Zone *z;
  char chnm1[12], resnm1[12], atmnm1[12], chnm2[12], resnm2[12], atmnm2[12];

  z = NEWTYPE (Zone);
  if (!z) {
    fprintf (stderr, "memory exhausted\n");
    return NULL;
  }

  atm_decode_atmnam (from, chnm1, resnm1, atmnm1);
  atm_decode_atmnam (to, chnm2, resnm2, atmnm2);

  z->ch1 = atm_find_named_chain (st, chnm1);
  z->ch2 = atm_find_named_chain (st, chnm2);
  if (!z->ch1) {
    fprintf (stderr, "warning: no chain named \"%s\"\n", chnm1);
    return NULL;
  }
  if (!z->ch2) {
    fprintf (stderr, "warning: no chain named \"%s\"\n", chnm2);
    return NULL;
  }

  z->res1 = atm_find_residue (z->ch1, resnm1);
  z->res2 = atm_find_residue (z->ch2, resnm2);
  if (!z->res1) {
    fprintf (stderr, "warning: no residue named \"%s\" in chain \"%s\"\n", 
	     resnm1, chnm1);
    return NULL;
  }
  if (!z->res2) {
    fprintf (stderr, "warning: no residue named \"%s\" in chain \"%s\"\n",
	     resnm2, chnm2);
    return NULL;
  }

  z->atm1 = atm_find_atom (z->res1, atmnm1);
  z->atm2 = atm_find_atom (z->res2, atmnm2);
  if (!z->atm1) {
    fprintf (stderr, "warning: no atom named \"%s\" in residue \"%s\"\n", 
	     atmnm1, resnm1);
    return NULL;
  }
  if (!z->atm2) {
    fprintf (stderr, "warning: no atom named \"%s\" in residue \"%s\"\n",
	     atmnm2, resnm2);
    return NULL;
  }

  z->nres = z->res2->ic - z->res1->ic +1;

#ifdef xxxx
  do the swapping here and return without error
  if (z->nres <= 0) {
    fprintf (stderr, "error in zone definition [%s %s], swap residues\n",
	     from, to);
    free (z);
    return NULL;
  }
#endif
  return z;
}

/**
   Pass atom residue of the beginning and end of a zone, and return a
   Zone datastructure containing the pointers.
*/
Zone *atm_residue_zone (Structure *st, char *from, char *to)
{
  Zone *z;
  char chnm1[12], resnm1[12], chnm2[12], resnm2[12];

  z = NEWTYPE (Zone);
  if (!z) {
    fprintf (stderr, "memory exhausted\n");
    return NULL;
  }

  atm_decode_resnam (from, chnm1, resnm1);
  atm_decode_resnam (to, chnm2, resnm2);

  z->ch1 = atm_find_named_chain (st, chnm1);
  z->ch2 = atm_find_named_chain (st, chnm2);

  if (!z->ch1) {
    fprintf (stderr, "warning: no chain named \"%s\"\n", chnm1);
    return NULL;
  }
  if (!z->ch2) {
    fprintf (stderr, "warning: no chain named \"%s\"\n", chnm2);
    return NULL;
  }

  z->res1 = atm_find_residue (z->ch1, resnm1);
  z->res2 = atm_find_residue (z->ch2, resnm2);
  if (!z->res1) {
    fprintf (stderr, "warning: no residue named \"%s\" in chain \"%s\"\n", 
	     resnm1, chnm1);
    return NULL;
  }
  if (!z->res2) {
    fprintf (stderr, "warning: no residue named \"%s\" in chain \"%s\"\n",
	     resnm2, chnm2);
    return NULL;
  }

  z->atm1 = NULL;
  z->atm2 = NULL;

  z->nres = z->res2->ic - z->res1->ic +1;
  if (z->nres <= 0) {
    fprintf (stderr, "error in zone definition [%s %s], swap residues\n",
	     from, to);
    free (z);
    return NULL;
  }
 return z;
}

/**
  Given an atom name on the form 'chain:res.atom' return a pointer to
  the atom - otherwise NULL.
*/

Atom *atm_find_named_atom (Structure *st, char *atomcode)
{
  char chnm[12], resnm[12], atmnm[12];
  Chain *ch;
  Residue *res;
  Atom *atm;

  atm_decode_atmnam(atomcode, chnm, resnm, atmnm);	
  ch = atm_find_named_chain (st, chnm);
  if (!ch) {
    fprintf (stderr, "warning: no chain named \"%s\"\n",chnm); 
    return NULL;
  }

  res = atm_find_residue (ch, resnm);
  if (!res) {
    fprintf (stderr, "warning: no residue named \"%s\" in chain \"%s\"\n", 
	     resnm, chnm);
    return NULL;
  }

  atm = atm_find_atom (res, atmnm);
  if (!atm) {
    fprintf (stderr, "warning: no atom named \"%s\" in residue \"%s\"\n", 
	     atmnm, resnm);
    return NULL;
  }

  return atm;
}

/**
  Delete a chain and free the memory.
*/

Chain *atm_delete_chain (Chain *chain)
{
  Residue *res, *nextres;
  Atom *atom, *nextatom;

  if (!chain)
    return NULL;

  res = chain->res;
  while (res) {
    nextres = res->next;
    atom = res->atoms;
    while (atom) {
      nextatom = atom->next;
      free(atom);
      atom = nextatom;
    }
    free(res);
    res = nextres;
  }
  free(chain);
  return NULL;
}

/**
  Delete a structure and free the memory
*/

Structure *atm_delete_structure (Structure *s)
{
  Chain *chain, *nextchain;

  if (!s)
    return NULL;

  chain = s->chain;
  while (chain) {
    nextchain = chain->next;
    chain = atm_delete_chain(chain);
    chain = nextchain;
  }

  /* here, free other datastructures that are part of a Structure. For now,
     there aren't any... */

  free(s);
  return NULL;
}




/** 
   Given a pointer to an atom, return the anisotropic temperature
   factor as a 3x3 matrix. The routine allocates a new Matrix3
   datastructure; it is the repsonsibility of the user to free this
   memory again.  mok 970801.
*/
Matrix3 *atm_aniso_to_M3 (Atom *atom)
{
  Matrix3 *m;

  m = NEWTYPE(Matrix3);
  m->element[0][0] = atom->aniso[0];
  m->element[1][1] = atom->aniso[1];
  m->element[2][2] = atom->aniso[2];
  m->element[0][1] = atom->aniso[3];
  m->element[0][2] = atom->aniso[4];
  m->element[1][2] = atom->aniso[5];
  m->element[1][0] = m->element[0][1];
  m->element[2][0] = m->element[0][2];
  m->element[2][1] = m->element[1][2];
}

/* 
   Local Variables:
   mode: font-lock
   End:
*/

