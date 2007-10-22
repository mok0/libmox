/*
 * $Id: psdriver.c 36 2006-01-25 13:36:15Z mok $
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
 *  PostScript driver routines for mgp.
 */ 

/* This 9" x 11" PostScript */
#ifdef LETTER
#define IN2PS (72.0)
#define XMIN 0.0		
#define XMAX 8.5*IN2PS
#define YMIN 0.0
#define YMAX 11.5*IN2PS
#define ZMIN 4.0
#define ZMAX 1.0
#define XMARGIN 0.15*IN2PS
#define YMARGIN 0.15*IN2PS
#endif

/* This is A4 */
#ifdef A4
#define CM2PS (72.0/2.54)
#define XMIN 0.0		
#define XMAX (21.0*CM2PS)
#define YMIN 0.0
#define YMAX (29.7*CM2PS)
#define ZMIN 4.0
#define ZMAX 1.0
#define XMARGIN (0.8*CM2PS)
#define YMARGIN (0.8*CM2PS)
#endif

#include "mgp.h"
#include "mgpdefs.h"

static int ctpage = 0;		/* counter for pages */
static int colourmode = RGB;

/*
  m g p _ b e g i n _ p l o t 
*/
void mgp_begin_plot (xmin,xmax,ymin,ymax)
     double xmin, xmax, ymin, ymax;
{
  char *s;
  long clock;
  extern char *ctime(), *getlogin();	/* ctime, getlogin (3-BSD) */
  extern long time();		        /* time (3C-BSD) */

  clock = time(0); s = ctime(&clock);

  printf ("%%!PS-Adobe-1.0\n");
  printf ("%%%%Title:\n");
  printf ("%%%%Creator: mgp by Morten Kjeldgaard <mok@imsb.au.dk>\n");
  printf ("%%%%For: %s\n",getlogin());
  printf ("%%%%CreationDate: %s", s);
  printf ("%%%%DocumentFonts: Times-Roman\n");
  printf ("%%%%BoundingBox: %.2lf %.2lf %.2lf %.2lf\n", xmin,ymin,xmax,ymax);
  printf ("%%%%EndComments\n");

  /* From O_plotat:
     % L -- draw a line of a certain thickness.
     % stack: x y n => ---
     % (C) Morten Kjeldgaard, 03-Jan-1990 */
  printf ("/Z { setlinewidth lineto currentpoint stroke moveto } bind def\n");
  printf ("/L { lineto } bind def\n");
  printf ("/M { moveto } bind def\n");
  printf ("%%%%EndProlog\n%%%%Page: ? %d\n",++ctpage); 
  printf ("save gsave 1 setlinewidth 1 setlinecap\n");
  printf ("[] 0 setdash\n");
  printf ("/Times-Roman findfont 12 scalefont setfont\n");
}


/*
  m g p _ l i n e t o
  Output a lineto instruction on stdout.
*/

void mgp_lineto (pointp, depth)
     Point3 *pointp;
     int depth;
{
  if (depth)
    printf ("%.2f %.2f %.2f Z\n", pointp->x, pointp->y, pointp->z);
  else
    printf ("%.2f %.2f L\n", pointp->x, pointp->y);    
}


/*
  m g p _ m o v e t o
  Output a moveto instruction on stdout.
*/
void mgp_moveto (pointp)
     Point3 *pointp;
{
  printf ("%.2f %.2f M\n", pointp->x, pointp->y);
}

/*
  m g p _ p o l y g o n _ b e g i n
  Output a polygon begin instruction on stdout.
*/
void mgp_polygon_begin ()
{
  printf ("gsave newpath\n");
}

/*
  m g p _ p o l y g o n _ b e g i n
  Output a polygon end instruction on stdout.
*/
void mgp_polygon_end ()
{
  printf ("closepath fill grestore\n");
}

/*
  m g p _ p o l y l i n e
  Output a polyline.
*/
mgp_polyline (n, depth, line)
     int n, depth;
     Point3 *line;
{
  register int i;

  printf ("%.2f %.2f M\n", line->x, line->y);
  line++;
  for (i=1; i < n; i++) {
    if (depth)
      printf ("%.2f %.2f %.2f Z\n", line->x, line->y, line->z);
    else
      printf ("%.2f %.2f L\n", line->x, line->y);
    line++;
  }
}



/*
  m g b _ s e t c o l
  Set the current drawing colour. mode = 0 for rgb, 1 for hsb.
  mok 930126.
*/
void mgp_setcol (mode,r,g,b) 
     int mode;
     float r,g,b;
{
  if (mode == 0)
    printf ("stroke %.3f %.3f %.3f setrgbcolor\n",r,g,b);
  else
    printf ("stroke %.3f %.3f %.3f sethsbcolor\n",r,g,b);
}

/*
  m g b _ c o l o u r
  Set the current drawing colour. mode = 0 for rgb, 1 for hsb.
  mok 930126.
*/
void mgp_colour (c)
     Colour *c;
{
  if (colourmode == RGB)
    printf ("stroke %.3f %.3f %.3f setrgbcolor\n",c->r,c->g,c->b);
  else
    printf ("stroke %.3f %.3f %.3f sethsbcolor\n",c->r,c->g,c->b);
}


/*
  l i n e t y p e
  set the linetype
*/
void mgp_linetype (mode)
     int mode;
{
  switch (mode) {
  case 1:			/* solid */
    printf ("stroke [] 0 setdash\n");
    break;
  case 2:			/* dashed */
    printf ("stroke [4] 0 setdash\n");
    break;
  case 3:			/* dotted */
    printf ("stroke [2] 0 setdash\n");
    break;
  case 4:			/* dash-dot */
    printf ("stroke [6 4 2 4] 0 setdash\n");
    break;
  }
}

/*
  m g p _ t e x t s i z e
  set the size of text to be drawn from now on
  mok 930126.
*/
void mgp_textsize (size)
     float size;
{
  printf ("/Times-Roman findfont %.2f scalefont setfont\n",12.0*size);
}

/*
  m g p _ s h o w t e x t
  Move to given position and display text.
  mok 930122.
*/
void mgp_showtext (pointp, str, depth)
     Point3 *pointp;
     char *str;
     int depth;			/* 1 if depth cueing, else 0 */
{

  if (depth) {
    printf ("/Times-Roman findfont %.2f scalefont setfont\n",  6.0*pointp->z);
    printf ("%.2f %.2f moveto (%s) show\n", pointp->x, pointp->y, str);
  } 
  else 
    printf ("%.2f %.2f moveto (%s) show\n", pointp->x, pointp->y, str);
}


/*
  m g p _ b e g i n o b j
*/

void mgp_beginobj (name)
     char *name;
{
  printf ("%% New Object -> %s\n",name);
  printf ("newpath\n");
}

/*
  m g p _ e n d o b j 
*/
void mgp_endobj ()
{
  printf ("stroke\n");
  printf ("%% End of Object\n"); 
}

/* 
m g p _ e n d p l o t 
End of plotting. Period. Finito. No more.
*/
void mgp_endplot()
{
  printf ("showpage grestore restore\n");
  printf ("%%%%Trailer\n");
}


/* 
   m g p _ n e w p a g e
   Give us a new page to plot on!
*/

void mgp_newpage()
{
  extern int ctpage;
  extern int Hidden;

  if (Hidden == 2) {
    printf ("newpage\n");
  }
  else {
    printf ("stroke showpage\n");
    printf ("%%%%Page: ? %d\n",++ctpage);
  }
}


/*
  ROUTINES RETURNING INFORMATION ABOUT THE DEVICE
  ===============================================
*/

/* 
   m g p _ g e t p l o t s i z e
   Return the size of the *plot* area in device units.
*/
void mgp_getplotsize(xmin,xmax,ymin,ymax,zmin,zmax)
     float *xmin,*xmax,*ymin,*ymax,*zmin,*zmax;
{
  *xmin = XMIN+XMARGIN; *xmax = XMAX-XMARGIN;
  *ymin = YMIN+YMARGIN; *ymax = YMAX-YMARGIN;
  *zmin = ZMIN; *zmax = ZMAX;
}


/* 
   m g p _ g e t p a g e s i z e
   Return the size of the *entire* page area in device units.
*/
void mgp_getpagesize(xmin,xmax,ymin,ymax,zmin,zmax,xmarg,ymarg)
     float *xmin,*xmax,*ymin,*ymax,*zmin,*zmax,*xmarg,*ymarg;
{

  *xmin = XMIN; *xmax = XMAX; *xmarg = YMARGIN;
  *ymin = YMIN; *ymax = YMAX; *ymarg = YMARGIN;
  *zmin = ZMIN; *zmax = ZMAX;

}

/* 
   m g p _ g e t p l o t a s p e c t
   Return the aspect ratio y/x of the plot area.
*/
float mgp_getplotaspect()
{
  return ((YMAX-YMARGIN)-(YMIN+YMARGIN))/((XMAX-XMARGIN)-(XMIN+XMARGIN));
}

/*** end of $Source$ ***/
