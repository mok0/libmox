/* 
 * $Id: clipper.c 36 2006-01-25 13:36:15Z mok $
 *
 * Copyright (C) 1993-1998 Morten Kjeldgaard <mok@imsb.au.dk>
 * All rights reserved.
 *
 * This software may be freely copied, modified, and redistributed
 * provided that this copyright notice is preserved on all copies.
 * If you modify this software, you should include a notice giving the
 * name of the person performing the modification, the date of modification,
 * and the reason for such modification.
 *
 * You may not distribute this software, in whole or in part, as part of
 * any commercial product without the express consent of the author.
 *
 * There is no warranty or other guarantee of fitness of this software
 * for any purpose. It is provided solely "as is".
 *
 */

/*
This file contains:
	csclipPar (p1, p2, clipflag)
	outcodePar (p, clipflag)
	csclipPer (p1, p2, zmin, clipflag)
	outcodePer (p, clipflag)
*/

#ifdef DEBUG
#include <stdio.h>
#endif

#define SWAP(a,b)       { a^=b; b^=a; a^=b; }
#define TRUE 1
#define FALSE 0

/*  

    c s c l i p P a r

    This is an implementation of the Cohen Sutherland clipper in 3 dimensions,
    for the PARALLEL projection geometry.  It's purpose is to determine whether
    the plane projection of the linesegment from p1 to p2 intersects the UNIT
    view volume.  The function returns true if the linesegment intersects the
    window, else false. The line visible inside the window is returned in p1,
    p2.  See Foley & van Dam p. 296.  
    mok 930120.  
*/

int csclipPar (p1, p2, clipflag)
     double p1[3], p2[3];	/* line endpoints */
     char clipflag;		/* flag for clipping */
{
  int code1, code2, i, itmp;
  double t;
  int outcodePar();		/* define function */

#ifdef DEBUG
  fprintf (stderr,"csclipPar: p1 = (%lf,%lf,%lf)\n", p1[0], p1[1], p1[2]);
  fprintf (stderr,"csclipPar: p2 = (%lf,%lf,%lf)\n", p2[0], p2[1], p2[2]);
#endif

  for (;;) {
    code1 = outcodePar(p1,clipflag);
    code2 = outcodePar(p2,clipflag);

    /* Trivially reject segments that are inside the window.
       If the logical "and" of the outcodes is != 0, we can trivally accept
       segment. */

    if (code1 == 0 && code2 == 0) {
#ifdef DEBUG
      prompt ("line segment ACCEPTED!\n");
#endif
      return TRUE;
    }
    else if ((code1 & code2) != 0) {
#ifdef DEBUG
      prompt ("line segment rejected\n");
#endif
      return FALSE;
    }

    /* If the result of the logical "and" is 0, the line can neither be 
       trivially rejected or accepted -- it may intersect the window.
       -- First, if p1 is inside the window, exchange p1 and p2 so that p1 is
       OUTSIDE the window. */

    if (code1 == 0) { 
      double tmp;
      SWAP(code1,code2);
      for (i=0; i<3; i++) {
	tmp = p1[i]; p1[i] = p2[i]; p2[i] = tmp;
      }
    }

    /* Now perform subdivision, move p1 to intersection point; use
       the formulas y = y1 + slope*(x-x1), x = x1 + (1/slope)*(y-y1) */

    if ((code1 & 32) != 0) {	/* bit 6 set, P1 is behind cube */
#ifdef DEBUG
      fprintf (stderr,"divide at behind\n");
#endif
      t = (1.0-p1[2])/(p2[2]-p1[2]);
      p1[0] = t * (p2[0]-p1[0]) + p1[0];
      p1[1] = t * (p2[1]-p1[1]) + p1[1];
      p1[2] = 1.0;
    }

    else if ((code1 & 16) != 0) { /* bit 5 set divide at near edge of window */
#ifdef DEBUG
      fprintf (stderr,"divide at front\n");
#endif
      t = -p1[2]/(p2[2]-p1[2]);
      p1[0] = t * (p2[0]-p1[0]) + p1[0];
      p1[1] = t * (p2[1]-p1[1]) + p1[1];
      p1[2] = 0.0;
    }

    else if ((code1 & 8) != 0) {	/* bit 4 set, divide at top of window */
#ifdef DEBUG
      fprintf (stderr,"divide at top\n");
#endif
      t = (1.0 - p1[1])/(p2[1]-p1[1]);
      p1[0] = t * (p2[0]-p1[0]) + p1[0];
      p1[1] = 1.0;
      p1[2] = t * (p2[2]-p1[2]) + p1[2];
    }

    else if ((code1 & 4) != 0) { /* bit 3 set, divide at bottom of window */
#ifdef DEBUG
      fprintf (stderr,"divide at bottom\n");
#endif
      t = -p1[1]/(p2[1]-p1[1]);
      p1[0] = t * (p2[0]-p1[0]) + p1[0];
      p1[1] = 0.0;
      p1[2] = t * (p2[2]-p1[2]) + p1[2];
    }

    else if ((code1 & 2) != 0) { /* bit 2 set divide at right edge of window */
#ifdef DEBUG
      fprintf (stderr,"divide at right\n");
#endif
      t = (1.0 - p1[0])/(p2[0]-p1[0]);
      p1[0] = 1.0;
      p1[1] = t * (p2[1]-p1[1]) + p1[1];
      p2[2] = t * (p2[2]-p1[2]) + p1[2];
    }

    else if ((code1 & 1) != 0) { /* bit 1 set, divide at left edge of window */
#ifdef DEBUG
      fprintf (stderr,"divide at left\n");
#endif
      t = -p1[0]/(p2[0]-p1[0]);
      p1[0] = 0.0;
      p1[1] = t * (p2[1]-p1[1]) + p1[1];
      p2[2] = t * (p2[2]-p1[2]) + p1[2];
    }
  }
}


/*
  o u t c o d e P a r
  Determine the outcode of the point r. The outcode depends on the
  following bitpatterns, where the central square is the clipping window,
  equivalent to outcode2D.
  Each bit is set in the outcode if a given relation between the endpoint
  and window is true:
        Bit 1 -- point is above view volume.
	Bit 2 -- point is below view volume.
	Bit 3 -- point is to the right of view volume.
	Bit 4 -- point is to the left of view volume.
	Bit 5 -- point is in front of view volume.
	Bit 6 -- point is behind view volume.
  otherwise, the bit is set to 0.
  Ref. Foley & van Dam p.146.
  mok 930120.
*/

int outcodePar (r,clipflag)
     double r[3];
     char clipflag;
{
  char code;
  
  code = 0;

  if (r[2] > 1.0)		/* is r behind view volume? */
    code = code | 32;
  if (r[2] < 0.0)		/* is r in front of view volume? */
    code = code | 16;
  if (r[1] > 1.0)		/* is r above view volume? */
    code = code | 8;
  if (r[1] < 0.0)		/* is r below view volume? */
    code = code | 4;
  if (r[0] > 1.0)		/* is r to the right of view volume? */
    code = code | 2;
  if (r[0] < 0.0)		/* is r to the left of view volume? */
    code = code | 1;

#ifdef DEBUG
  fprintf (stderr,"outcode: %f %f %f %d\n", r[0], r[1], r[2], code); 
#endif
  return (code^clipflag)&code;	/* mask out bits according to flag */
}


/*
  c s c l i p P e r
  This is an implementation of the Cohen Sutherland clipper in 3 dimensions,
  for the PERSPECTIVE projection.  It's purpose is to determine whether the
  plane projection of the linesegment from p1 to p2 intersects the UNIT view
  volume (pyramid).  The function returns true if the linesegment intersects the
  window, else false. The line visible inside the window is returned in p1,
  p2.  See Foley & van Dam p. 296.  
  mok 930120.
*/

int csclipPer (p1, p2, zmin, clipflag)
     double p1[3], p2[3], zmin;	/* line endpoints */
     char clipflag;		/* flags for clipping front & back */
{
  int code1, code2, i, itmp;
  double t;
  int outcodePer();		/* define function */

#ifdef DEBUG
  fprintf (stderr,"csclipPer: p1 = (%lf,%lf,%lf)\n", p1[0], p1[1], p1[2]);
  fprintf (stderr,"csclipPer: p2 = (%lf,%lf,%lf)\n", p2[0], p2[1], p2[2]);
#endif

  for (;;) {
    code1 = outcodePer(p1,zmin,clipflag);
    code2 = outcodePer(p2,zmin,clipflag);

    /* Trivially reject segments that are inside the window.
       If the logical "and" of the outcodes is != 0, we can trivally accept
       segment. */

    if (code1 == 0 && code2 == 0) {
#ifdef DEBUG
      prompt ("line segment ACCEPTED!\n");
#endif
      return TRUE;
    }
    else if ((code1 & code2) != 0) {
#ifdef DEBUG
      prompt ("line segment rejected\n");
#endif
      return FALSE;
    }

    /* If the result of the logical "and" is 0, the line can neither be 
       trivially rejected or accepted -- it may intersect the window.
       -- First, if p1 is inside the window, exchange p1 and p2 so that p1 is
       OUTSIDE the window. */

    if (code1 == 0) { 
      double tmp;
      SWAP(code1,code2);
      for (i=0; i<3; i++) {
	tmp = p1[i]; p1[i] = p2[i]; p2[i] = tmp;
      }
    }

    /* Now perform subdivision, move p1 to intersection point; use
       the formulas y = y1 + slope*(x-x1), x = x1 + (1/slope)*(y-y1) */

    if ((code1 & 32) != 0) {	/* bit 6 set, P1 is behind volume */
#ifdef DEBUG
      fprintf (stderr,"divide at behind\n");
#endif
      t = (1.0 - p1[2])/(p2[2]-p1[2]);
      p1[0] = t * (p2[0]-p1[0]) + p1[0];
      p1[1] = t * (p2[1]-p1[1]) + p1[1];
      p1[2] = 1.0;
    }

    else if ((code1 & 16) != 0) { /* bit 5 set divide at near edge of window */
#ifdef DEBUG
      fprintf (stderr,"divide at front\n");
#endif
      t = (zmin - p1[2])/(p2[2]-p1[2]);
      p1[0] = t * (p2[0]-p1[0]) + p1[0];
      p1[1] = t * (p2[1]-p1[1]) + p1[1];
      p1[2] = zmin;
    }

    else if ((code1 & 8) != 0) {	/* bit 4 set, divide at top of window */
#ifdef DEBUG
      fprintf (stderr,"divide at top\n");
#endif
      t = (p1[2]-p1[1])/((p2[1]-p1[1])-(p2[2]-p1[2]));
      p1[2] = t * (p2[2]-p1[2]) + p1[2];
      p1[0] = t * (p2[0]-p1[0]) + p1[0];
      p1[1] = p1[2];
    }

    else if ((code1 & 4) != 0) { /* bit 3 set, divide at bottom of window */
#ifdef DEBUG
      fprintf (stderr,"divide at bottom\n");
#endif
      t = (p1[2]-p1[1])/((p2[1]-p1[1])-(p2[2]-p1[2]));
      p1[2] = t * (p2[2]-p1[2]) + p1[2];
      p1[0] = t * (p2[0]-p1[0]) + p1[0];
      p1[1] = -p1[2];
    }

    else if ((code1 & 2) != 0) { /* bit 2 set divide at right edge of window */
#ifdef DEBUG
      fprintf (stderr,"divide at right\n");
#endif
      t = (p1[2]-p1[0])/((p2[0]-p1[0])-(p2[2]-p1[2]));
      p1[2] = t * (p2[2]-p1[2]) + p1[2];
      p1[0] = p1[2];
      p1[1] = t * (p2[1]-p1[1]) + p1[1];
    }

    else if ((code1 & 1) != 0) { /* bit 1 set, divide at left edge of window */
#ifdef DEBUG
      fprintf (stderr,"divide at left\n");
#endif
      t = (p1[2]+p1[0])/((p1[0]-p2[0])-(p2[2]-p1[2]));
      p1[2] = t * (p2[2]-p1[2]) + p1[2];
      p1[0] = -p1[2];
      p1[1] = t * (p2[1]-p1[1]) + p1[1];
    }
  }
}



/*
  o u t c o d e P y r
  Determine the outcode of the point r. The outcode depends on the
  following bitpatterns, where the central square is the clipping window,
  equivalent to outcode2D.
  Each bit is set in the outcode if a given relation between the endpoint
  and window is true:
	Bit 1 -- point is above view volume.
	Bit 2 -- point is below view volume.
	Bit 3 -- point is to the right of view volume.
	Bit 4 -- point is to the left of view volume.
	Bit 5 -- point is in front of view volume.
	Bit 6 -- point is behind view volume.
  otherwise, the bit is set to 0.
  Ref. Foley & van Dam p.146.
  mok, 930120
*/

int outcodePer (r, zmin, clipflag)
     double r[3], zmin;		/* endpoint */
     char clipflag;
{
  char code;
  
  code = 0;

  if (r[2] > 1.0)		/* is r behind view volume? */
    code = code | 32;
  if (r[2] < zmin)		/* is r in front of view volume? */
    code = code | 16;
  if (r[1] > r[2])		/* is r above view volume? */
    code = code | 8;
  if (r[1] < -r[2])		/* is r below view volume? */
    code = code | 4;
  if (r[0] > r[2])		/* is r to the right of view volume? */
    code = code | 2;
  if (r[0] < -r[2])		/* is r to the left of view volume? */
    code = code | 1;

#ifdef DEBUG
  fprintf (stderr,"outcode: %f %f %f %d\n", r[0], r[1], r[2], code); 
#endif

  return (code^clipflag)&code;	/* mask out bits according to flag. */
}



#ifdef TESTING
#define PRINTP1P2 printf ("p1=(%f,%f,%f) , p2=(%f,%f,%f)\n", \
			  p1[0], p1[1], p1[2], p2[0], p2[1], p2[2])
main () {

   double  p1[3],p2[3];

   int csclipPar ();

   for (;;) {
      printf ("Window is [0:1; 0:1; 0:1]\n");
      printf ("Enter two points, p1 & p2: ");
      scanf ("%f %f %f %f %f %f", &p1[0],&p1[1],&p1[2], &p2[0], &p2[1], &p2[2]);
      PRINTP1P2;

      if (csclipPar(p1,p2,1,1)) {
         printf ("+++++++++++++++++LINE INTERSECTS\n");
	 PRINTP1P2;
         }
      else {
         printf ("-----------------Line doesn''t intersect\n");
	 PRINTP1P2;
         }
      }
 }
#endif /* TESTING */


/* end of clipper.c */
