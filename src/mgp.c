
#include <stdio.h>
#include <string.h>		/* define strcpy and friends */
#include <math.h>		/* for cos & sin */
#if defined (sgi) || defined(__linux__) 
# include <values.h>		/* to define MAXFLOAT */
#endif

#include "mgp.h"
#include "mgpdefs.h"
#define EPS 1.0e-6
#ifndef NULL
#  define NULL (0)
#endif

/* Macro to see if colour has changed */
#define CheckColour(A,B) { \
      if ( ((fabs(A.r - B.r)) > 0.01) ||  \
	   ((fabs(A.g - B.g)) > 0.01) ||  \
	   ((fabs(A.b - B.b)) > 0.01) ) { \
	mgp_colour (&B); \
	A.r = B.r; \
	A.g = B.g; \
	A.b = B.b; \
      } }


/* Macro to copy struct */
#define StructCopy(A,B) { \
     bcopy ((char *)&A, (char *)&B, sizeof(B)); \
     }

/* Macro to coerce float to be non-negative */
#define POS(x) { if ((x) < 0.0) (x) = 0.0; }

/* Global variables */
static Point3 vrp = {0.0,0.0,0.0};      /* View Reference Point */
static Vector3 vpn = {0.0,0.0,-1.0};    /* View plane normal  */
static Vector3 dop = {0.0,0.0,1.0};     /* direction of projection (parallel) */
static Point3 cop = {0.0,0.0,-1.0};     /* center of projection (perspective) */
static Vector3 vup = {0.0,1.0,0.0};     /* view up direction */
static Point2 window_lo = {0.0,0.0};    /* corners of the view plane window */
static Point2 window_hi = {1.0,1.0};
static Point3 viewport3_lo = {0.0,0.0,0.0};  /* corners of the 3D viewport */
static Point3 viewport3_hi = {1.0,1.0,1.0};
static Point3 device_lo = {5.0,5.0,4.0};       /* Window in physical device */
static Point3 device_hi = {605.0,605.0,1.0};   /* coordinates. */
static int view_mode = PARALLEL; /* viewing mode (parallel or perspective) */

static Matrix4 T =    { 1.0,0.0,0.0,0.0,     /* View Translation matrix */
		        0.0,1.0,0.0,0.0,
		        0.0,0.0,1.0,0.0,
		        0.0,0.0,0.0,1.0 };
static Matrix4 R =    { 1.0,0.0,0.0,0.0,     /* View Rotation matrix */
		        0.0,1.0,0.0,0.0,
		        0.0,0.0,1.0,0.0,
		        0.0,0.0,0.0,1.0 };
static Matrix4 Trl =  { 1.0,0.0,0.0,0.0,     /*  */
		        0.0,1.0,0.0,0.0,
		        0.0,0.0,-1.0,0.0,
		        0.0,0.0,0.0,1.0 };
static Matrix4 SHz =  { 1.0,0.0,0.0,0.0,     /* Shearing matrix */
		        0.0,1.0,0.0,0.0,
		        0.0,0.0,1.0,0.0,
		        0.0,0.0,0.0,1.0 };
static Matrix4 Spar = { 1.0,0.0,0.0,0.0,     /* Scale to unit cube */
		        0.0,1.0,0.0,0.0,
		        0.0,0.0,1.0,0.0,
		        0.0,0.0,0.0,1.0 };
static Matrix4 Tpar = { 1.0,0.0,0.0,0.0,     /* Translate to unit cube */
		        0.0,1.0,0.0,0.0,
		        0.0,0.0,1.0,0.0,
		        0.0,0.0,0.0,1.0 };
static Matrix4 Sper = { 1.0,0.0,0.0,0.0,     /* Scale to pyramid */
		        0.0,1.0,0.0,0.0,
		        0.0,0.0,1.0,0.0,
		        0.0,0.0,0.0,1.0 };
static Matrix4 Tper = { 1.0,0.0,0.0,0.0,     /* Translate to pyramid */
		        0.0,1.0,0.0,0.0,
		        0.0,0.0,1.0,0.0,
		        0.0,0.0,0.0,1.0 };
static Matrix4 MTrf = { 1.0,0.0,0.0,0.0,     /* Modelling transformation */
			0.0,1.0,0.0,0.0,    
		        0.0,0.0,1.0,0.0,
		        0.0,0.0,0.0,1.0 };
				/* slab for clipping */
static float Front = 0.0, Back = 1.0;

static int PlotInit = 0;	/* Has plot been initialized? */
				/* Current Point */
static Point3 CP = { 0.0,0.0,0.0 };
				/* The graphics environment */
GraphicsContext GC = {1,1.0,12.0,{0.0,0.0,0.0},0};

static LightSource Lights[4]; 
/* {1,{1.0,1.0,1.0},{0.0,0.0,-10.0},1.0,0.0,0.0,0.0,0.0 }; */

static View CurrentView = {
  { 1.0,0.0,0.0,0.0,		/* Np */
    0.0,1.0,0.0,0.0, 
    0.0,0.0,1.0,0.0,
    0.0,0.0,0.0,1.0 },
  { 1.0,0.0,0.0,0.0,		/* Ip */
    0.0,1.0,0.0,0.0, 
    0.0,0.0,1.0,0.0,
    0.0,0.0,0.0,1.0 },
  { 1.0,0.0,0.0,0.0,		/* N2D */
    0.0,1.0,0.0,0.0, 
    0.0,0.0,1.0,0.0,
    0.0,0.0,0.0,1.0 },
    48,				/* ClipFlag, no clipping if bits are set */
    0.0};			/* PerZmin */

static int NumberLights = 1;	/* Number of lights defined */

/* External variables, can be set by user */
int Verbose = 0;		/* If set, print more stuff */
int Debug = 0;			/* ... and even more... */
int GhostScript = 0;		/* If set, display with gs */
int Hidden = 0;			/* Hidden line & surface mode */
int BlackAndWhite = 0;		/* If set, black and white only */
int Fog = 0;			/* Fog effect in shading */
int Shading;			/* = 0, polygon shading is off */

Object *CurrentObject = NULL;	/* Pointer to currently open object */
Object *RootObject = NULL;	/* Bogus root object for backwards compat. */
				/* Settings outside objects are stored here. */

/***** BUILD ROUTINES ******/


/* 
   B u i l d V i e w i n g T r a n s f o r m 
   Calculate the 4x4 matrix describing the normalizing transformation, that
   maps the world coordinates into the unit cube.
   mok 930120.
*/

void BuildViewingTransform()
{
  Vector3 rx, ry, rz, dop1, vrp1, cw;
  Matrix4 M4;

  /* Define R matrix. Common to parallel & perspective projections */

  rz.x = -vpn.x ; rz.y = -vpn.y ; rz.z = -vpn.z;
  v3_normalize(&rz);
  v3_cross(&vpn, &vup, &rx);
  v3_normalize (&rx);  
  v3_cross(&rz, &rx, &ry);
  R.element[0][0] = rx.x; R.element[0][1] = ry.x;  R.element[0][2] = rz.x;
  R.element[1][0] = rx.y; R.element[1][1] = ry.y;  R.element[1][2] = rz.y; 
  R.element[2][0] = rx.z; R.element[2][1] = ry.z;  R.element[2][2] = rz.z;

  if (view_mode == PERSPECTIVE) {

    /* Define Tper */
    Tper.element[3][0] = -(vrp.x+cop.x);
    Tper.element[3][1] = -(vrp.y+cop.y);
    Tper.element[3][2] = -(vrp.z+cop.z);

    /* Determine VRP' */
    m3_matmul (&Tper, &R, &M4);
    m3_matmul (&M4, &Trl, &CurrentView.Np);
    v3_mulPointByProjMatrix (&vrp, &CurrentView.Np, &vrp1);

    /* Determine center-of-window CW */
    cw.x = vrp1.x + 0.5*(window_lo.x + window_hi.x);
    cw.y = vrp1.y + 0.5*(window_lo.y + window_hi.y);
    cw.z = vrp1.z;

    /* Now define shearing matrix SHz */
    SHz.element[2][0] = -cw.x/cw.z;
    SHz.element[2][1] = -cw.y/cw.z;
    
    /* Define Sper, that scales to canonical volume */
    Sper.element[0][0] = 2.0*vrp1.z/((window_hi.x-window_lo.x)*(vrp1.z+Back));
    Sper.element[1][1] = 2.0*vrp1.z/((window_hi.y-window_lo.y)*(vrp1.z+Back));
    Sper.element[2][2] = 1.0/(vrp1.z+Back);

    /* Finally apply shear & scale to Nper: */
    m3_matmul (&CurrentView.Np, &SHz, &M4);
    m3_matmul (&M4, &Sper, &CurrentView.Np);

    CurrentView.PerZmin = (vrp1.z+Front)/(vrp1.z+Back); /* F & V p.295 */
/*    PerZproj = vrp1.z/(vrp1.z+Back); */

  } else if (view_mode == PARALLEL) {

    /* 1. Define T matrix translates vrp to origin: */
    T.element[3][0] = -vrp.x;
    T.element[3][1] = -vrp.y;
    T.element[3][2] = -vrp.z;
  
    /* 5. Build shearing matrix SHz */
    m3_matmul (&R, &Trl, &M4);
    v3_mulPointByProjMatrix (&dop, &M4, &dop1);
    SHz.element[2][0] = -dop1.x/dop1.z;
    SHz.element[2][1] = -dop1.y/dop1.z;

    /* 6. Transform sheared volume into unit cube */

    Tpar.element[3][0] = -window_lo.x; 
    Tpar.element[3][1] = -window_lo.y;
    Tpar.element[3][2] = -Front;

    Spar.element[0][0] = 1.0/(window_hi.x-window_lo.x);
    Spar.element[1][1] = 1.0/(window_hi.y-window_lo.y);
    Spar.element[2][2] = 1.0/(Back-Front);

    m3_matmul (&T, &R, &CurrentView.Np);
    m3_matmul (&CurrentView.Np, &Trl, &M4);
    m3_matmul (&M4, &SHz, &CurrentView.Np);
    m3_matmul (&CurrentView.Np, &Tpar, &M4);
    m3_matmul (&M4, &Spar, &CurrentView.Np);
  }
  else
    prompt ("BuildViewingTransform: unknown mode\n");

/*  m4_print ("Np set:",&CurrentView.Np); */

}


/*
  B u i l d V i e w p o r t M a p p i n g
  Create the 4x4 matrix that maps the unit cube coordinates to the
  3D viewport (NDC coordinates).
  mok 930121
*/

void BuildViewportMapping()
{ 

  if (view_mode == PERSPECTIVE) {
    Matrix4 M, Ta, Sa, T1, tmp, *m4_ident();

    m4_ident(&M); m4_ident(&Ta); m4_ident(&Sa); m4_ident(&T1);
    M.element[2][2] = 1.0/(1.0-viewport3_lo.z);
    M.element[3][2] = -viewport3_lo.z/(1.0-viewport3_lo.z);
    M.element[2][3] = 1.0;
    M.element[3][3] = 0.0;
    Ta.element[3][0] = 1.0; Ta.element[3][1] = 1.0;
    Sa.element[0][0] = 0.5*(viewport3_hi.x - viewport3_lo.x);
    Sa.element[1][1] = 0.5*(viewport3_hi.y - viewport3_lo.y);
    Sa.element[2][2] = viewport3_hi.z - viewport3_lo.z;
    T1.element[3][0] = viewport3_lo.x;
    T1.element[3][1] = viewport3_lo.y;
    T1.element[3][2] = viewport3_lo.z;

    /* Now form Iper = M.Ta.Sa.T1 (p. 302) */
    m3_matmul (&M, &Ta, &CurrentView.Ip);
    m3_matmul (&CurrentView.Ip, &Sa, &tmp);
    m3_matmul (&tmp, &T1, &CurrentView.Ip);

  } else if (view_mode == PARALLEL) {

    m4_ident (&CurrentView.Ip);
    CurrentView.Ip.element[0][0] = (viewport3_hi.x - viewport3_lo.x);
    CurrentView.Ip.element[1][1] = (viewport3_hi.y - viewport3_lo.y);
    CurrentView.Ip.element[2][2] = (viewport3_hi.z - viewport3_lo.z);
    CurrentView.Ip.element[3][0] = viewport3_lo.x;
    CurrentView.Ip.element[3][1] = viewport3_lo.y;
    CurrentView.Ip.element[3][2] = viewport3_lo.z;
  } 
  else 
    prompt ("BuildViewportMapping: unknown mode\n");

/*  m4_print ("Ip set:",&CurrentView.Ip); */

}

/*
  B u i l d D e v i c e M a p p i n g
  Build the transformation matrix that takes us from normalized device 
  coordinates to device coordinates.
  mok 930120.
*/
static void BuildDeviceMapping ()
{
  float xmin, xmax, ymin, ymax, zfar, znear;
  float mgp_getplotaspect();

  /* Ask device driver for size of paper (SHOULD GO IN INIT ROUTINE!!)*/
  mgp_getplotsize (&xmin, &xmax, &ymin, &ymax, &zfar, &znear);

  if (mgp_getplotaspect() > 1.0) /* define a square plotting area */
    ymax = ymin + (xmax-xmin);
  else
    xmax = xmax + (ymax-ymin);

  device_lo.x = xmin;
  device_hi.x = xmax;
  device_lo.y = ymin;
  device_hi.y = ymax;

  CurrentView.N2D.element[0][0] = (device_hi.x-device_lo.x);
  CurrentView.N2D.element[1][1] = (device_hi.y-device_lo.y);
  CurrentView.N2D.element[2][2] = (device_hi.z-device_lo.z);
  CurrentView.N2D.element[3][0] = device_lo.x;
  CurrentView.N2D.element[3][1] = device_lo.y;
  CurrentView.N2D.element[3][2] = device_lo.z;


}

/* 
   T r a n s f o r m _ a n d _ c l i p _ T e x t
   Transform to canonical volume, clip, project, and transform to device
   coordinates.
   mok 931115.
*/
int Transform_and_Clip_Text (primp, textp)
     Primitive *primp;
     TextPrim *textp;
{
  Point3 x1, x2;
  Matrix4 M;


  m3_matmul (&MTrf, &CurrentView.Np, &M);
  v3_mulPointByProjMatrix (&textp->where, &M, &x1);
  primp->zmin = x1.z;

  if (view_mode == PERSPECTIVE) { 
    if (!csclipPer( &x1, &x2, CurrentView.PerZmin, CurrentView.ClipFlag))
      return 0;
  } else if (view_mode == PARALLEL) {
    if (!csclipPar(&x1, &x1, CurrentView.ClipFlag))
      return 0;
  } else
    prompt ("Transform_and_Clip_Text: unknown mode\n");
  
  m3_matmul (&CurrentView.Ip, &CurrentView.N2D, &M);
  v3_mulPointByProjMatrix (&x1, &M, &textp->where);
  
  return 1;
}

/* 
   T r a n s f o r m _ a n d _ c l i p _ L i n e
   Transform to a line canonical volume, clip, project, and transform to device
   coordinates.
   mok 931115.
*/
int Transform_and_Clip_Line (primp, linep)
     Primitive *primp;
     LinePrim *linep;
{
  Point3 x1, x2;
  Matrix4 M;
  

  m3_matmul (&MTrf, &CurrentView.Np, &M);
  v3_mulPointByProjMatrix (&linep->from, &M, &x1);
  v3_mulPointByProjMatrix (&linep->to  , &M, &x2);
  primp->zmin = MAX(x1.z, x2.z); /* update zmin for this primitive */

  if (view_mode == PERSPECTIVE) { 
    if (!csclipPer( &x1, &x2, CurrentView.PerZmin, CurrentView.ClipFlag))
      return 0;
  } else if (view_mode == PARALLEL) {

    if (!csclipPar(&x1, &x2, CurrentView.ClipFlag))
      return 0;

  } else
    prompt ("Transform_and_Clip_Line: unknown mode\n");

  m3_matmul (&CurrentView.Ip, &CurrentView.N2D, &M);
  v3_mulPointByProjMatrix (&x1, &M, &linep->from);
  v3_mulPointByProjMatrix (&x2, &M, &linep->to);
  return 1;

}

/* 
   T r a n s f o r m _ a n d _ C l i p _ P o l y
   Transform to canonical volume, clip, project, and transform to device
   coordinates a polygon.
   mok 930219.
*/
int Transform_and_Clip_Poly (primp, polyp)
     Primitive *primp;
     PolyPrim *polyp;
{
  register int i;
  Point3 x1, *P1;
  Matrix4 M1, M2;

  if (polyp == NULL) {
    prompt ("NULL polygon\n");
    return 0;
  }

  /* Transform centre and normal by modelling transformation. */

  m3_matmul (&MTrf, &CurrentView.Np, &M1);
  v3_mulPointByProjMatrix (&polyp->centre, &M1, &x1 ); /* &MTrf, &x1); */
  polyp->centre.x = x1.x;  polyp->centre.y = x1.y;  polyp->centre.z = x1.z;
  v3_mulPointByMatrix4 (&polyp->normal.vec, &M1, &x1);
  polyp->normal.vec.x = x1.x;  
  polyp->normal.vec.y = x1.y;  
  polyp->normal.vec.z = x1.z;

  v3_normalize(&polyp->normal.vec);

  primp->zmin = -MAXFLOAT;
  m3_matmul (&MTrf, &CurrentView.Np, &M1);
  m3_matmul (&CurrentView.Ip, &CurrentView.N2D, &M2);
  primp->zmin = -MAXFLOAT;
  P1 = polyp->data;

    for (i = 0; i < polyp->nvert; i++) {
      v3_mulPointByProjMatrix (P1, &M1, &x1);
      primp->zmin = MAX(primp->zmin, x1.z);

      if (view_mode == PERSPECTIVE) { 

      /* here comes perspective clipping routine 
 *    if (!csclipPer( &x1, &x2, CurrentView.PerZmin, CurrentView.ClipFlag))
 *        return 0; */

      } else if (view_mode == PARALLEL) {

      /* here comes polygon clipping routine
 *        if (!csclipPar(&x1, &x2, CurrentView.ClipFlag))
 *           return 0; */

      } else
	prompt ("Transform_and_Clip_Poly: unknown mode\n");

      v3_mulPointByProjMatrix (&x1, &M2, P1);
      P1++;
    }  
  return 1;
}


/*
  r e s e t _ t o _ d e f a u l t
  Reset all matrices and viewing parameters to default values.
  mok 930122, 930216.
*/
void reset_to_default ()
{
  v3_set (&vrp,0.0,0.0,0.0);	/* view reference point at origin */
  v3_set (&vpn,0.0,0.0,-1.0);	/* view point normal along -z */
  v3_set (&dop,0.0,0.0,1.0);	/* direction of projection along +z */
  v3_set (&cop,0.0,0.0,-1.0);	/* center of projection at z = -1 */
  v3_set (&vup,0.0,1.0,0.0);	/* view-up vector along y */
  v2_set (&window_lo,0.0,0.0);	/* low window corner */
  v2_set (&window_hi,1.0,1.0);	/* high window corner */
  v3_set (&viewport3_lo,0.0,0.0,0.0); 
  v3_set (&viewport3_hi,1.0,1.0,1.0);
  v3_set (&device_lo,5.0,5.0,4.0); /* plotting area for PostScript page */
  v3_set (&device_hi,605.0,605.0,1.0); /* last number is linewidth */
  v3_set (&CP,0.0,0.0,0.0);	/* current point at origin */
  view_mode = PARALLEL;		/* parallel projection is default */
  Front = 0.0; Back = 1.0;	/* Front, back clipping planes */
  CurrentView.ClipFlag = 48;	/* bits 5,6 set means no front,back clipping */

  /* set misc matrices to identity */
  m4_ident (&T); m4_ident (&R); m4_ident (&SHz); m4_ident (&Spar); 
  m4_ident (&Tpar); m4_ident (&CurrentView.Np); m4_ident (&Sper); m4_ident (&Tper); 
  m4_ident (&CurrentView.Np); m4_ident (&CurrentView.Ip); 
  m4_ident (&CurrentView.Ip); m4_ident (&CurrentView.N2D); 
  m4_ident (&MTrf);

  BuildViewingTransform();
  BuildViewportMapping();
  BuildDeviceMapping();

  GC.zthick = 0; GC.linetype = 1; GC.linewidth = 1.0; GC.charsize = 12.0;
  GC.colour.r = GC.colour.g = GC.colour.b = 0.0;
  
  {
    Object *CreateNamedObject();
    RootObject = CreateNamedObject ("Bogus_root");
  }
}

/*
  R e s e t V i e w P a r
  Reset the view parameter data structure to default state.
*/
void ResetViewPar (viewp)
     ViewPar *viewp;
{
  v3_set (&viewp->vrp,0.0,0.0,0.0);	/* view reference point at origin */
  v3_set (&viewp->vpn,0.0,0.0,-1.0);	/* view point normal along -z */
  v3_set (&viewp->dop,0.0,0.0,1.0);	/* direction of projection along +z */
  v3_set (&viewp->cop,0.0,0.0,-1.0);	/* center of projection at z = -1 */
  v3_set (&viewp->vup,0.0,1.0,0.0);	/* view-up vector along y */
  v2_set (&viewp->window.min,0.0,0.0);	/* low window corner */
  v2_set (&viewp->window.max,1.0,1.0);	/* high window corner */
  v3_set (&viewp->viewport.min,0.0,0.0,0.0); 
  v3_set (&viewp->viewport.max,1.0,1.0,1.0);
  v3_set (&viewp->device.min,5.0,5.0,4.0);/* plotting area for PostScript page */
  v3_set (&viewp->device.max,605.0,605.0,1.0); /* last number is linewidth */
  viewp->ViewMode = PARALLEL;		/* parallel projection is default */
  v2_set (&viewp->Clip, 0.0, 1.0);	/* Front, back clipping planes */
  viewp->ClipFlag = 48;	/* bits 5,6 set means no front,back clipping */
}

/*
  L I G H T  R O U T I N E S 
*/

#define RGB2CMY(r) (1.0-r)
/* #define BLACKBGND /* */
/* 
   P o l y S h a d e
   Given surface colour, normal, and type, and the number of lamps,
   return a pointer to the shaded colour of the surface.
*/
static void PolyShade (col, pos, N, V, surf, nlight, lights, newcol)
     Colour *col, *newcol;	/* surface colour, apparent colour */
     Vector3 *pos, *N, *V;	/* surface position, normal, viewpoint */
     Surface *surf;		/* surface type */
     int nlight;		/* number of light sources */
     LightSource lights[];	/* array of light sources */
{
  int j;
  float d, d0, LdotN, RdotV;
  float c, m, y;
  float La;
  Vector3 L, LL, B;
  Colour2 I, Ia, Ip, ka, kd;

  d0 = 1.0;
  I.c = I.m = I.y = 0.0;

  /* convert colour to (c,m,y) */

  if (!Fog) {
    c = (col->r);  m = (col->g);  y = (col->b);  
  } 
  else {
    c = (1.0 - col->r);  m = (1.0 - col->g);  y = (1.0 - col->b);
  }

  ka.c = c * surf->ambient;  
  ka.m = m * surf->ambient;  
  ka.y = y * surf->ambient;
  kd.c = c * surf->diffuse;  
  kd.m = m * surf->diffuse;  
  kd.y = y * surf->diffuse;


/*
  fprintf (stderr, "kd = %f %f %f \n", kd.c,kd.m,kd.y);
  fprintf (stderr, "ka = %f %f %f \n", ka.c,ka.m,ka.y);
*/

  /* Loop through lightsources */

  for (j=0; j<nlight; j++) {

    d0=1.0 ; /* ? */

    Ip.c = Ia.c = (lights[j].col.r) * lights[j].intensity ;
    Ip.m = Ia.m = (lights[j].col.g) * lights[j].intensity ;
    Ip.y = Ia.y = (lights[j].col.b) * lights[j].intensity ;

    /* Vector from surface to lightsource */
    LL.x = lights[j].position.x;
    LL.y = lights[j].position.y;
    LL.z = lights[j].position.z;

    v3_sub (pos,&LL, &L); 
    d = v3_length (&L);	/* Distance from light source to surface */
    v3_normalize(&L);
    LdotN = (v3_dot (N, &L));
    POS(LdotN);

#ifdef GGmethod
    c = L.x*V->x + L.y*V->y + L.z*V->z + 1.0; /* -ve here ?? */
    RdotV = (c/sqrt(c+c));
#else
    v3_add (V, &L, &B); v3_normalize (&B);
/*    RdotV = fabs(v3_dot (N, &B)); */
    RdotV = v3_dot (N, &B);
    POS(RdotV);
#endif

    d0 = surf->specular * pow((double)RdotV, surf->pow);

    switch (lights[j].type) {
    case DIRECTIONAL:
      I.c += Ia.c * ka.c + Ip.c * (kd.c * LdotN +  d0);
      I.m += Ia.m * ka.m + Ip.m * (kd.m * LdotN +  d0);
      I.y += Ia.y * ka.y + Ip.y * (kd.y * LdotN +  d0);
      break;
      
    case POSITIONAL:
      La = 1.0/(lights[j].atten[0] + lights[j].atten[1]*d);
      I.c += Ia.c * ka.c + La * Ip.c * (kd.c * LdotN + d0);
      I.m += Ia.m * ka.m + La * Ip.m * (kd.m * LdotN + d0);
      I.y += Ia.y * ka.y + La * Ip.y * (kd.y * LdotN + d0);
      break;

    case SPOT:
      d0 = surf->specular * pow((double)RdotV, surf->pow);
      I.c += Ia.c * ka.c + Ip.c * (kd.c * LdotN + d0);
      I.m += Ia.m * ka.m + Ip.m * (kd.m * LdotN + d0);
      I.y += Ia.y * ka.y + Ip.y * (kd.y * LdotN + d0);
      break;

    default:
      prompt ("No such light source type");

    }
  }

 /* Convert back to (r,g,b) */

  if (!Fog) {
    newcol->r = I.c;  newcol->g = I.m;  newcol->b = I.y;
  }
  else {
    newcol->r = 1.0 - I.c;  newcol->g = 1.0 - I.m;  newcol->b = 1.0 - I.y;
  }
 CLAMP(newcol->r, 0.0, 1.0);
 CLAMP(newcol->g, 0.0, 1.0);
 CLAMP(newcol->b, 0.0, 1.0);

}

/*
 * VIEWLIST ROUTINES 
 * ==================
 */

/* 
   A d d O b j V i e w
   Create a new viewing parameter in given objects list 
   mok 931121
*/

ObjViewList *AddObjView (optr) 
     Object *optr;
{
  
  ObjViewList *viewp, *p;
  
  if (optr == NULL) {
    prompt ("AddObjView: Null object\n");
    return NULL;
  }

  viewp = NEWTYPE (ObjViewList);  /* Create new viewing block */
  viewp->next = NULL;

/*  fprintf (stderr,"AddObjView\n"); /* DBG */
	
  /* Link this record to the end of the list */
  
  if (optr->viewlist == NULL)
    optr->viewlist = viewp;
  else {
    p = optr->viewlist;
    while (p->next) {		/* go to end of list */
/*      fprintf (stderr, "%d ", p->type);    /* dbg */
      p = p->next;
    }
    p->next = viewp;
  }

  return viewp;
}
	

/* 
   D e l e t e O b j V i e w
   Delete object viewing parameter list from given object
   mok 931121
*/

void DeleteObjView (optr)
     Object *optr;
{
  
  ObjViewList *p, *q;
  
  if (optr == NULL)
    return;

  if (optr->viewlist != NULL) {
    p = optr->viewlist;

    do {			/* free till end of list */
      q = p->next;
      free (p);
      p = q;
    } while (p);
  }
}
	

/* 
   S e t O b j V i e w
   Go through an objects list of parameters and set them in the viewing parameter
   data structure.
   mok 931121
*/
int SetViewPar (optr, viewpar) 
     Object *optr;
     ViewPar *viewpar;
{
  ObjViewList *p;

  if (optr == NULL)
    return 0;

  p = optr->viewlist;

  while (p) {

    switch (p->type) {
    case 1:			/* View reference point */
      v3_copy (&p->data.vrp, &viewpar->vrp);
      break;
    case 2:			/* View plane normal */
      v3_copy (&p->data.vpn, &viewpar->vpn);
      break;

    case 3:			/* Direction of projection (parallel) */
      v3_copy (&p->data.dop, &viewpar->dop);
      break;

    case 4:			/* Center of projection (Perspective) */
      v3_copy (&p->data.cop, &viewpar->cop);
      break;

    case 5:			/* View up direction */
      v3_copy (&p->data.vup, &viewpar->vup);
      break;

    case 6:			/* View plane window */
      v2_copy (&p->data.window.min, &viewpar->window.min);
      v2_copy (&p->data.window.max, &viewpar->window.max);
      break;

    case 7:			/* Viewport */
      v3_copy (&p->data.viewport.min, &viewpar->viewport.min);
      v3_copy (&p->data.viewport.max, &viewpar->viewport.max);
      break;

    case 8:			/* Device viewport */
      v3_copy (&p->data.device.min, &viewpar->device.min);
      v3_copy (&p->data.device.max, &viewpar->device.max);
      break;

    case 9:			/* Viewing mode (Parallel or perspective) */
      viewpar->ViewMode = p->data.ViewMode;
      break;

    case 10:			/* Clip flag */
      viewpar->ClipFlag = p->data.ClipFlag;
      break;

    case 11:			/* Front and back clipping limits */
      v2_copy (&p->data.Clip, &viewpar->Clip);
      break;

    default:
      fprintf (stderr,"SetViewPar: no such viewing parameter: %d\n",p->type);
    }
    p = p->next;
  }

  return 1;
}

/* 
   CopyGC -- Copy the graphics context.
   mok 980529
*/

CopyGC (GraphicsContext *src, GraphicsContext *dest)
{
  if (src && dest) {
    bcopy ( src, dest, sizeof(GraphicsContext));
  }
}

/*
 * PARAMETER SETTING ROUTINES
 * ==========================
 */

/*
  s e t _ p r o j e c t i o n
  Set the projection type (parallel, perspective). In the first case,
  set the direction-of-projection, in the second case, set the 
  center-of-projection (eye position). The center-of-projection is set
  relative to the view-reference point.
  mok 930119.
*/
void set_projection (mode, x, y, z)
     int mode;
     float x,y,z;
{
  Vector3 v;
  ObjViewList *viewp;

  v.x = x; v.y = y; v.z = z;

  switch (mode) {

  case PARALLEL:
    if (v3_squaredlength (&v) < EPS) {
      prompt ("Direction of projection must be <> (0,0,0)\n");
      return;
    }
    v3_copy (&v, &dop);
    if (Verbose)
      v3_print ("Direction of parallel projection set", &dop);

    if (CurrentObject != NULL) { 
      viewp = AddObjView (CurrentObject);
    } else {
      prompt ("No open object\n");
      viewp = AddObjView (RootObject);
    }
    viewp->type = 3;
    v3_copy (&v, &viewp->data.dop);
    break;

  case PERSPECTIVE:
    cop.x = x; cop.y = y; cop.z = z;
    if (Verbose)
      v3_print ("Center of perspective projection set", &cop);

    if (CurrentObject != NULL) { 
      viewp = AddObjView (CurrentObject);
    } else {
      prompt ("No open object\n");
      viewp = AddObjView (RootObject);
    } 
    viewp->type = 4;
    v3_copy (&v, &viewp->data.cop);
    break;
  }

  if (CurrentObject != NULL) {
    viewp = AddObjView (CurrentObject);
  } else {
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 9;
  viewp->data.ViewMode = mode;

  view_mode = mode;

  BuildViewingTransform();
  BuildViewportMapping();
  BuildDeviceMapping();
}

/* 
   s e t _ v i e w _ r e f e r e n c e _ p o i n t
   Set the view reference point (look at point).
   mok 930119.
*/
void set_view_reference_point (x,y,z)
     float x,y,z;
{
  ObjViewList *viewp;

  vrp.x = x; vrp.y = y; vrp.z = z;
  if (Verbose)
    v3_print ("View reference point set", &vrp);

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 1;
  v3_copy (&vrp, &viewp->data.vrp);

  BuildViewingTransform();
}


/* 
   s e t _ v i e w _ p l a n e _n o r m a l  
   Set the view plane normal.
   mok 930119.
*/
void set_view_plane_normal (x,y,z)
     float x,y,z;
{
  Vector3 v;
  ObjViewList *viewp;

  v.x = x; v.y = y; v.z = z;

  if (v3_squaredlength (&v) < EPS) {
    prompt ("View plane normal must be <> (0,0,0)\n");
    return;
  }
  v3_copy (&v, &vpn);
  if (Verbose)
    v3_print ("View plane normal set", &dop);

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 2;
  v3_copy (&v, &viewp->data.vpn);

  BuildViewingTransform();
}

/*
  s e t _ v i e w _ u p
  Set the view-up direction.
  mok 930119.
*/
void set_view_up (x,y,z)
     float x,y,z;
{
  Vector3 v;
  ObjViewList *viewp;

  v.x = x; v.y = y; v.z = z;

  if (v3_squaredlength (&v) < EPS) {
    prompt ("View up direction must be <> (0,0,0)\n");
    return;
  }
  v3_copy (&v, &vup);
  if (Verbose)
    v3_print ("View up direction set", &vup);

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 5;
  v3_copy (&v, &viewp->data.vrp);

  BuildViewingTransform();
}


/* 
   s e t _ w i n d o w 
   Set the extent of the viewing window, in the viewing plane.
   mok 930119.
*/

void set_window (xlo, xhi, ylo, yhi)
     float xlo,xhi,ylo,yhi;
{
  ObjViewList *viewp;

  if (fabs(xhi-xlo) > EPS) 
    window_lo.x = xlo; window_hi.x = xhi;
  if (fabs(yhi-ylo) > EPS) 
    window_lo.y = ylo; window_hi.y = yhi;

  if (Verbose)
    fprintf (stderr, "Window set: %f:%f %f:%f\n", window_lo.x, window_hi.x,
	     window_lo.y, window_hi.y);

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 6;
  v2_copy (&window_hi, &viewp->data.window.max);
  v2_copy (&window_lo, &viewp->data.window.min);

  BuildViewingTransform();
}

/* 
   s e t _ v i e w p o r t_3
   Set the 3D viewport in the NDC space.
   mok 930121.
*/

void set_viewport_3 (xlo, xhi, ylo, yhi, zlo, zhi)
     float xlo,xhi,ylo,yhi,zlo,zhi;
{
  ObjViewList *viewp;

  if (fabs(xhi-xlo) > EPS)
    viewport3_lo.x = xlo; viewport3_hi.x = xhi;
  if (fabs(yhi-ylo) > EPS)
    viewport3_lo.y = ylo; viewport3_hi.y = yhi;
  if (fabs(zhi-zlo) > EPS)
    viewport3_lo.z = zlo; viewport3_hi.z = zhi;

  if (Verbose)
    fprintf (stderr,"3D viewport set: %f:%f %f:%f %f:%f\n",
	     viewport3_lo.x, viewport3_hi.x,
	     viewport3_lo.y, viewport3_hi.y,
	     viewport3_lo.z, viewport3_hi.z);

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 7;
  v3_copy (&viewport3_hi, &viewp->data.viewport.max);
  v3_copy (&viewport3_lo, &viewp->data.viewport.min);

  BuildViewportMapping();
}

/* 
   s e t _ v i e w p o r t_2
   Set the viewport size for 2D plotting.
   mok 930122.
*/

void set_viewport_2 (xlo, xhi, ylo, yhi)
     float xlo,xhi,ylo,yhi;
{
  ObjViewList *viewp;

  if (fabs(xhi-xlo) > EPS)
    viewport3_lo.x = xlo; viewport3_hi.x = xhi;
  if (fabs(yhi-ylo) > EPS)
    viewport3_lo.y = ylo; viewport3_hi.y = yhi;

  if (Verbose)
    fprintf (stderr,"2D viewport set: %f:%f %f:%f\n",
	     viewport3_lo.x, viewport3_hi.x,
	     viewport3_lo.y, viewport3_hi.y);

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 7;
  v3_copy (&viewport3_hi, &viewp->data.viewport.max);
  v3_copy (&viewport3_lo, &viewp->data.viewport.min);

  BuildViewportMapping();
}


/*
  s e t _ v i e w _ d e p t h 
  Set the depth of view, or the positions of the front & back clipping plances.
  mok 930120.
*/
set_view_depth (hither, yon)
     float hither, yon;
{
  ObjViewList *viewp;

  Front = hither; Back = yon;
  if (Front > Back) {
    double tmp;
    tmp = Back; Back = Front; Front = tmp;
    }

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 11;
  viewp->data.Clip.x = Front;
  viewp->data.Clip.y = Back;

  if (Verbose)
    fprintf (stderr, "View depth set: Front %f Back %f\n", 
	     viewp->data.Clip.x, viewp->data.Clip.y);

  BuildViewingTransform();

}


/*
  s e t _ f r o n t _ p l a n e _ c l i p p i n g
  Turn on/off front plane clipping. If 5 bit is set, it means that no
  front plane clipping will take place.
  mok 930122
*/
set_front_plane_clipping (on_off)
     int on_off;
{
  ObjViewList *viewp;

  if (Verbose)
    fprintf (stderr, "Front plane clipping %d [%d -> ", on_off,  CurrentView.ClipFlag);

  if (on_off)
    CurrentView.ClipFlag &= ~(1<<4); /* unset bit 5 */
  else
    CurrentView.ClipFlag |= 1<<4;    /* set bit 5 */

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 10;
  viewp->data.ClipFlag = CurrentView.ClipFlag;
  if (Verbose)
    fprintf (stderr, "%d]\n", CurrentView.ClipFlag);
}

/*
  s e t _ b a c k _ p l a n e _ c l i p p i n g
  Turn on/off back plane clipping. If bit 6 is set, it means that no
  back plane clipping will take place.
  mok 930123
*/
void set_back_plane_clipping (on_off)
     int on_off;
{
  ObjViewList *viewp;

  if (Verbose)
    fprintf (stderr, "Back plane clipping %d [%d -> ", on_off,  CurrentView.ClipFlag);

  if (on_off)
    CurrentView.ClipFlag &= ~(1<<5);	/* unset bit 6 */
  else
    CurrentView.ClipFlag |= 1<<5;		/* set bit 6 */

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 10;
  viewp->data.ClipFlag = CurrentView.ClipFlag;
  if (Verbose)
    fprintf (stderr, "%d]\n", CurrentView.ClipFlag);

}

/*
  s e t _ c l i p p i n g _ f l a g 
  This routine allows you to set the clipping flag directly.
  The 6 low bits correspond to clipping at left, right, bottom, top, 
  front, back. If the bit is set, no clipping takes place. 
  NB: use of this routine potentially gives portability problems!
  mok 930218
*/
void set_clipping_flag (i)
     int i;
{
  ObjViewList *viewp;

  CurrentView.ClipFlag = i;

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 10;
  viewp->data.ClipFlag = i;

}


/*
  s e t _ d e p t h _ c u e i n g
  Turn on/off depth cueing.
  mok 930125
*/
void set_depth_cueing (on_off)
     int on_off;
{
  GC.zthick = on_off;
}

/*
  s e t _ z t h i c k 
  Set the zthickening parameters
  mok 930515
*/
void set_zthick (front, back)
     float front, back;
{
  ObjViewList *viewp;

  device_lo.z = front;
  device_hi.z = back;
  if (Verbose)
    fprintf (stderr,"Zthickening set: Front = %f, Back = %f\n", front, back);

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 8;
  v3_copy (&device_hi, &viewp->data.device.max);
  v3_copy (&device_lo, &viewp->data.device.min);
  BuildDeviceMapping();
}


/* 
   s e t _ d e v i c e _ a r e a
   Set the 3D plotting area of the device (z is typically line-thickness).
   mok 930125.
*/
void set_device_area (xlo, xhi, ylo, yhi, zlo, zhi)
     float xlo,xhi,ylo,yhi,zlo,zhi;
{
  ObjViewList *viewp;

  if (fabs(device_hi.x-device_lo.x) > EPS) {
    device_lo.x = xlo; device_hi.x = xhi;
  }
  if (fabs(device_hi.y-device_lo.y) > EPS) {
    device_lo.y = ylo; device_hi.y = yhi;
  }
  if (fabs(device_hi.y-device_lo.y) > EPS) {
    device_lo.z = zlo; device_hi.z = zhi;
  }  

  if (CurrentObject != NULL) { 
    viewp = AddObjView (CurrentObject);
  } else { 
    prompt ("No open object\n");
    viewp = AddObjView (RootObject);
  }
  viewp->type = 8;
  v3_copy (&device_hi, &viewp->data.device.max);
  v3_copy (&device_lo, &viewp->data.device.min);

  BuildDeviceMapping();
}




/*
  s e t _ m o d e l l i n g _ t r a n s f o r m a t i o n
  Set the modelling transformation, which is applied to world coordinates
  before anything else.
*/
set_modelling_transformation (M,mode)
     Matrix4 *M;
     int mode;
{
  int i,j;
  Matrix4 Rr;
  extern Matrix4 *m4_copy();

  switch (mode) {
  case PRE_CONCAT:		/* preconcat */
    m4_copy(&MTrf, &Rr);
    m3_matmul (M,&Rr,&MTrf);
    break;
  case POST_CONCAT:		/* postconcat */
    m4_copy(&MTrf, &Rr);
    m3_matmul(&Rr, M, &MTrf);
    break;
  case REPLACE:			/* replace */
    for (i=0; i<4; i++)
      for(j=0; j<4; j++)
	MTrf.element[i][j] = M->element[i][j];
    break;
  default:
    prompt ("set_modelling_transformation: unknown mode\n");
  }

}

/*
  s e t _ m o d e l l i n g _ r o t a t i o n
  Set the modelling rotation matrix.
  mok 930122.
*/
set_modelling_rotation (M, mode)
     Matrix3 *M;
     int mode;
{
  int i,j;
  Matrix4 Mrot;

  m4_ident(&Mrot);
  for (i=0; i<3; i++)
    for(j=0; j<3; j++)
      Mrot.element[i][j] = M->element[i][j];

  set_modelling_transformation (&Mrot, mode);
}

/*
  s e t _ m o d e l l i n g _ s c a l e _ f a c t o r s
  Set the modelling scaling matrix.
  mok 930122.
*/
set_modelling_scale_factors (sx,sy,sz)
     float sx,sy,sz;
{
  Matrix4 Msca;

  m4_ident(&Msca); 
  Msca.element[0][0] = sx; 
  Msca.element[1][1] = sy; 
  Msca.element[2][2] = sz;

  set_modelling_transformation (&Msca, POST_CONCAT);
}

/*
  s e t _ m o d e l l i n g _ t r a n s l a t i o n
  Set the modelling translation.
  mok 930122
*/
set_modelling_translation (tx,ty,tz)
     float tx,ty,tz;
{
  Matrix4 M;
  Vector3 Mtra;

  v3_set(&Mtra, tx, ty, tz);
  m4_ident(&M); 
  M.element[3][0] = tx;
  M.element[3][1] = ty;
  M.element[3][2] = tz;

  set_modelling_transformation (&M, POST_CONCAT);
  
}

/*
  s e t _ m o d e l l i n g _ a n g l e
  Apply a rotation around the given axis. Angle given in degrees.
  mok 930217
*/
set_modelling_angle (axis, angle)
     char axis;
     float angle;
{
  Matrix4 M;
  double ca, sa, ang;

  m4_ident(&M);
  ang = angle*3.141592654/180.0;
  ca = cos (ang); sa = sin(ang);

  switch (axis) {
  case 'z':
    M.element[0][0] = ca;
    M.element[0][1] = sa;
    M.element[1][0] = -sa;
    M.element[1][1] = ca;
    break;
  case 'y':
    M.element[0][0] = ca;
    M.element[0][2] = -sa;
    M.element[2][0] = sa;
    M.element[2][2] = ca;
    break;
  case 'x':
    M.element[1][1] = ca;
    M.element[1][2] = sa;
    M.element[2][1] = -sa;
    M.element[2][2] = ca;
    break;
  default:
    prompt ("set_modelling_angle: bogus axis\n");
  }

  if (Verbose) {
    fprintf (stderr, "Applying rotation around %c axis, %f degrees\n",
	     axis, angle);
    m4_print ("Rotation matrix:", &M);
  }

  set_modelling_transformation (&M, POST_CONCAT);

  if (Debug) { m4_print ("The modelling tranformation is now:", &MTrf); }

}


/*
  s e t _ r g b _ c o l o u r
  Set the rgb colour value. 
*/
void set_rgb_colour (r,g,b)
     float r,g,b;
{
  if (BlackAndWhite)
    return;

  GC.colour.r = r;
  GC.colour.g = g;
  GC.colour.b = b;

  if (Verbose == 2)
    fprintf (stderr, "Colour set to: %f %f %f\n", r, g, b);
}

/*
  s e t _ l i n e _ r e p r e s e n t a t i o n
  set the line type: solid (=1), dashed (=2), dotted (=3), dot-dash (=4).
  mok 930126.
*/
void set_line_representation (mode, i, j)
     int mode,i,j;
{
  GC.linetype = mode;

  if (Verbose)
    fprintf (stderr, "Linetype set to: %d\n", mode);

}

/*
  s e t _ t e x t _ r e p r e s e n t a t i o n
  set the text parameters.
  mok 930126.
*/
void set_text_representation (size, i, j)
     float size;
     int i,j;
{
  GC.charsize = size;

/*  if (Verbose > 0)
    fprintf (stderr, "Charsize set to: %f\n", size); */

}

/*
  a c t i v a t e _ p l o t
  Initialize the plotter.
  mok930310
*/
void activate_plot ()
{
  Point3 P;

  if (!PlotInit) {
    mgp_begin_plot (device_lo.x, device_hi.x, device_lo.y, device_hi.y);
    PlotInit = 1;

    if (GhostScript) {		/* initalize page with black field. */
      float xmin,xmax,ymin,ymax,zmin,zmax,xmarg,ymarg;

      mgp_getpagesize (&xmin,&xmax,&ymin,&ymax,&zmin,&zmax,&xmarg,&ymarg);
      mgp_polygon_begin();
      mgp_setcol (0,0.0,0.0,0.0);
      v3_set (&P, xmin, ymin, 0.0);  mgp_moveto (&P);
      v3_set (&P, xmax, ymax, 0.0) ; mgp_lineto (&P, 0);
      v3_set (&P, xmin, ymax, 0.0) ; mgp_lineto (&P, 0);
      v3_set (&P, xmin, ymin, 0.0) ; mgp_lineto (&P, 0);
      mgp_polygon_end();
    }
  }
}

/*
  d e a c t i v a t e _ p l o t
  Wrap up plotting.
  mok930310
*/
void deactivate_plot ()
{
  if (PlotInit)
    mgp_endplot();
}


/*
 * OBJECT ROUTINES 
 * ===============
 */

Object *TopObject = NULL, *LastObject = NULL;
Object *DisplayList = NULL;

/*
  F i n d O b j e c t
  Return a pointer to the named object, else NULL.
  mok 930205
*/
static Object *FindObject (name)
     char *name;
{
  Object *p;

  p = TopObject;
  
  while (p != NULL) {
    if (strncasecmp(name, p->name, 24) == 0)
      break;
    p = p->next;
  }
  
  return p;
}


/*
  P r i n t O b j e c t L i s t
  Print the list of objects. For debugging.
  mok 930218
*/
void PrintObjectList ()
{
  Object *p;

  p = TopObject;
  while (p != NULL) {
    fprintf (stderr, "%s ", p->name);
    p = p->next;
  }
  fprintf (stderr, "\n");
}


/*
  D e l e t e O b j e c t
  Delete a named object. Well, actually we don't completely remove it,
  just trim off the branches, to maintain connectivity in the object
  list. Then we don't need a doubly linked structure.
  mok 930218.
*/
static Object *DeleteObject (optr)
     Object *optr;
{
  Primitive *pptr, *oldp;
  TextPrim *textp;
  LinePrim *linep;
  PolyPrim *polyp;
  Instance *iptr, *oldi;

  if (optr == NULL)
    return NULL;

/* Traverse list of primitives and free them */
  pptr = optr->prim_head;
  while (pptr != NULL) {
    switch (pptr->type) {
    case 1:
      linep = (LinePrim *)pptr->data; /* free the LinePrim  */
      free ((char *) linep);
      break;
    case 2:
      textp = (TextPrim *)pptr->data;
      free (textp->s);		/* free the string */
      free ((char *) textp);	/* free the TextPrim */
      break;
    case 3:			/* Polygons */
      polyp = (PolyPrim *)pptr->data;
      free ((char *) polyp->data); /* free the list of vertices */
      free ((char *) polyp);
      break;
    case 4:			/* Spheres */
      prompt ("spheres not yet implemented\n");
      break;
    default:
      prompt ("DeleteObject: cannot handle primitive\n");
    }
    oldp = pptr;
    pptr = pptr->next;		/* next primitive */
    free ((char *) oldp);	/* free the old one */
  }

/* Now traverse list of instances and free them */

  iptr = optr->inst_head;	/* first object in instance list */
  while (iptr != NULL) {
    if (iptr->trf != NULL) {	/* if there's a transformation, free it */
      free ((char *) iptr->trf);
      iptr->trf = NULL;
    }
    if (iptr->GC) {		/* free the graphics context */
      free ((char *) iptr->GC);
      iptr->GC = NULL;
    }

    oldi = iptr;
    iptr = iptr->next;		/* do next object in instance list */
    free ((char *) oldi);
  }

  /* Free the object view transformation, if any */

  DeleteObjView (optr);
  free(optr->GC);		/* free GC */
  free(optr->name);		/* free name string */

  optr->prim_head = NULL;
  optr->prim_tail = NULL;
  optr->inst_head = NULL;
  optr->inst_head = NULL;
  
  return optr;
}


/*
  C r e a t e N a m e d O b j e c t
  Create an object with a given name.
  mok 930127.
*/
Object *CreateNamedObject (name)
     char *name;
{
  Object *new;
  int exists;
  extern Matrix4 *m4_copy();
  extern char *strsave();

  exists = 0;
  new = FindObject(name);

  if (new != NULL) {
    exists = 1;
    DeleteObject (new);		/* object exists already, zap it*/
    if (Verbose)
      prompts ("Overwriting object ",name);
  } else {
    new = NEWSTRUCT(Object);	/* no object of this name existed */
    if (Verbose) 
      prompts ("Creating object ", name);
    new->viewlist = NULL;
  }

  if (TopObject == NULL) {	/* if first object, initialize TopObject */
    TopObject = new;
    LastObject = new;
  } else {			/* if not first object, link it into list */
    if (!exists) {		/* ... but only if it did not exist already */
      LastObject->next = new;
      LastObject = new;
    }
  }

  new->name = strsave(name);
  new->visible = 1;		/* default visibility on */
  new->GC = NEWSTRUCT(GraphicsContext);

  if (!exists) {
    new->next = NULL;		/* pointer to next object in list */
    new->inst_head = NULL;	/* pointer to list of instances */
    new->inst_tail = NULL;	/* pointer to last in list of instances */
    new->prim_head = NULL;	/* pointer to first primitive of this obj */
    new->prim_tail = NULL;	/* pointer to last primitive of this obj */
  }
    
  if (Debug) {
    prompt ("Current list of objects\n");
    PrintObjectList();
  }

  return new;
}


/* 
   C r e a t e P r i m i t i v e
   Insert a primitive in a given object, and return pointer.
   mok 930205
*/
static Primitive *CreatePrimitive(obj)
     Object *obj;
{
  Primitive *new;

  new = NEWSTRUCT(Primitive);

  if (obj->prim_head == NULL)	/* If object was empty, set the head pt. */
    obj->prim_head = new;

  if (obj->prim_tail != NULL)	/* If object is not empty */
    obj->prim_tail->next = new;	/* link into list of primitives */

  obj->prim_tail = new;		/* update the object's tail */

  new->next = NULL;		/* Initalize new primitive */
  new->type = 0;
  new->data = NULL;

  return new;
}

/*
  C r e a t e I n s t a n c e
  In a given object, create an instance transformation of an object.
  mok 930108
*/
static Instance *CreateInstance (obj, name, m, gc)
     Object *obj;
     char *name;
     Matrix4 *m;
     GraphicsContext *gc;
{
  Instance *new;
  Object *optr;
  extern Matrix4 *m4_copy();

  new = NEWSTRUCT(Instance);

  optr = FindObject(name);	/* find pointer to the object */
  if (optr == NULL) {
    if (Verbose)
      prompts("Creating empty object: ", name);
    optr = CreateNamedObject(name);
  }

  new->obj = optr;		/* pointer to object 'name' */
  new->trf = m4_copy(m, NEWTYPE(Matrix4));
  new->next = NULL;
  new->GC = NEWSTRUCT(GraphicsContext);
  CopyGC (gc, new->GC);

  /* Now link this instance into the object's list of instances. */

  if (obj->inst_head == NULL) {
    obj->inst_head = new;
    obj->inst_tail = new;
    return new;
  }

  obj->inst_tail->next = new;
  obj->inst_tail = new;

  return new;
}

/* --- line primitives --- */

/* 
   A d d L i n e 
   Add a line segment to the given object.
   mok 930205
*/
static void AddLine (obj, p1, p2)
     Object *obj;
     Point3 *p1, *p2;
{
  Primitive *prim;
  LinePrim *line;

  if (obj == NULL) {
    prompt ("AddLine: no open object\n");
    return;
  }

  prim = CreatePrimitive(obj);
  line = NEWSTRUCT(LinePrim);
  prim->data = (void *) line;
  prim->type = 1;		/* Primitive type is line-segment */

  line->col.r = GC.colour.r;
  line->col.g = GC.colour.g;
  line->col.b = GC.colour.b;

  line->type = GC.linetype;
  line->from.x = p1->x;
  line->from.y = p1->y;
  line->from.z = p1->z;
  line->to.x = p2->x;
  line->to.y = p2->y;
  line->to.z = p2->z;
/*  prim->zmin = MIN(p1->z,p2->z);  /* ??? kan den slettes ?? */
}

/* 
   D u p L i n e
   Duplicate a line segment, return pointer.
   mok 930506.
*/
static LinePrim *DupLine (linep)
     LinePrim *linep;
{
  register LinePrim *new;

  new = NEWSTRUCT(LinePrim);
  bcopy ((char *)linep, (char *)new, sizeof(LinePrim));
  return new;
}

/* 
   D e l e t e  L i n e 
   Delete a Linesegment.
   mok 930506.
*/
static void DeleteLine (linep)
     LinePrim *linep;
{
  free (linep);
}

/* --- polygon primitives --- */

/* 
   A d d P o l y g o n
   Add a polygon to the given object.
   mok 930219.
*/
static void AddPolygon (obj, ct, points)
     Object *obj;
     int ct;			/* number of vertices */
     Point3 *points;		/* address of array */
{
  Primitive *prim;
  PolyPrim *polyp;
  register Point3 *pp;
  register int i;
  float zmin;

  zmin = -MAXFLOAT;

  if (obj == NULL) {
    prompt ("AddPolygon: no open object\n");
    return;
  }

  prim = CreatePrimitive(obj);
  polyp = NEWSTRUCT(PolyPrim);
  prim->data = (void *) polyp;
  prim->type = POLYGON;		/* Primitive type is polygon */

  polyp->col.r = GC.colour.r;
  polyp->col.g = GC.colour.g;
  polyp->col.b = GC.colour.b;

  polyp->nvert = ct;
  polyp->data = pp = (Point3 *)calloc (ct, sizeof(Point3));
  if (pp != NULL) {
    for (i = 0; i < ct; i++) {
/*      zmin = MIN(zmin,points->z); */
      polyp->centre.x += points->x;
      polyp->centre.y += points->y;
      polyp->centre.z += points->z;
      v3_copy(points++, pp++);
      }
  }

  /* Determine centre of polygon */
  polyp->centre.x /= ct;  polyp->centre.y /= ct;  polyp->centre.z /= ct;

  prim->zmin = zmin;

/* calculate normal vector */

  if (planeq (polyp->nvert, polyp->data, polyp->normal.abcd) < 0)
    prompt ("Error calculating plane coefficients\n");
}


/* 
   D u p P o l y g o n
   Duplicate a Polygon, return pointer.
   mok 930219.
*/
static PolyPrim *DupPolygon (polyp)
     PolyPrim *polyp;
{
  PolyPrim *new;
  register int i;

  new = NEWSTRUCT(PolyPrim);

  /* Duplicate number of vertices */
  new->nvert = polyp->nvert;

  /* Duplicate the colour */
  StructCopy (polyp->col, new->col); 
  
  /* Copy the centre */
  StructCopy (polyp->centre, new->centre);

  /* Duplicate the normal */
  for (i=0; i<4; i++) {	
    new->normal.abcd[i] = polyp->normal.abcd[i];
  }

  /* Duplicate vertex coordinates */
  new->data = (Point3 *)calloc (new->nvert, sizeof(Point3));
  if (new->data != NULL) {
    bcopy ((char *)polyp->data, (char *)new->data, new->nvert*sizeof(Point3)); 
      }

  return new;
}

/* 
   D e l e t e  P o l y g o n
   Delete a Polygon.
   mok 930219.
*/
static void DeletePolygon (polyp)
     PolyPrim *polyp;
{
  free (polyp->data);
  free (polyp);
}

/* --- text primitives --- */

/* 
   A d d T e x t
   Add some text to the given object
*/
static void AddText (obj, p, text)
     Object *obj;
     Point3 *p;
     char *text;
{
  Primitive *prim;
  TextPrim *t;
  char *strsave();

  if (obj == NULL) {
    prompt ("AddText: no open object\n");
    return;
  }
  prim = CreatePrimitive(obj);
  t = NEWSTRUCT(TextPrim);
  prim->data = (void *)t;
  prim->type = TEXTSEGMENT;	/* primitive type is string */

  t->col.r = GC.colour.r;
  t->col.g = GC.colour.g;
  t->col.b = GC.colour.b;

  t->size = GC.charsize;
  t->s = strsave(text);
  t->where.x = p->x;
  t->where.y = p->y;
  t->where.z = p->z;

  prim->zmin = p->z;
}

/* 
   D u p T e x t
   Duplicate a Text primitive, return pointer.
   mok 930506.
*/
static TextPrim *DupText (textp)
     TextPrim *textp;
{
  TextPrim *new;
  extern char *strsave();

  new = NEWSTRUCT(TextPrim);

  /* copy size, colour, position */
  bcopy ((char *)textp, (char *)new, sizeof (TextPrim));

  /* Copy string content */
  new->s = strsave(textp->s);

  return new;
}

/* 
   D e l e t e T e x t
   Delete a Text primitive.
   mok 930506.
*/
static void DeleteText (textp)
     TextPrim *textp;
{
  free (textp->s);
  free (textp);
}


/*
  D i s p l a y O b j e c t 
  Traverse the given object and its children. This is a simple traversal
  which does not allow for hidden surface elimination. To obtain that,
  a more sophisticated traversal is needed, using a display list. See 
  the AddToDisplaylist and DisplayDisplayList routines.
  930215 mok.
*/
Object *DisplayObject (optr)
     Object *optr;
{
  Instance *iptr;
  Primitive *pptr;
  LinePrim *linep, *tmpline;
  TextPrim *textp, *tmptext;
  PolyPrim *polyp, *tmppoly;

  if (optr == NULL) {
    prompts ("Object not found\n");
    return;
  }

  if (Verbose)
    prompts ("Displaying object ",optr->name);

  CopyGC(optr->GC, &GC);
  mgp_linetype (GC.linetype);
  mgp_setcol (0,GC.colour.r, GC.colour.g, GC.colour.b);
  mgp_textsize (GC.charsize);

  if (optr->visible) {		/* visibility test */
    pptr = optr->prim_head;
    mgp_beginobj(optr->name);

    while (pptr != NULL) {
      
      switch (pptr->type) {
	
      case LINESEGMENT:		/* Line Segments */

	linep = (LinePrim *)pptr->data;
	tmpline = DupLine(linep); /* Don't overwrite original copy */

	if (Transform_and_Clip_Line (pptr, tmpline)) { 
	  if (linep->type != GC.linetype) {
	    mgp_linetype (linep->type);
	    GC.linetype = linep->type;
	  }
	  CheckColour (GC.colour,linep->col);
	  mgp_moveto (&tmpline->from);
	  mgp_lineto (&tmpline->to, GC.zthick);

	  DeleteLine (tmpline);
	}
	break;
 
      case TEXTSEGMENT:		/* Text segments */

	textp = (TextPrim *)pptr->data;
	if (!GC.zthick) {
	  if (textp->size != GC.charsize) {
	    mgp_textsize (textp->size);
	    GC.charsize = textp->size;
	  }
        }
      
	tmptext = DupText (textp);

	if (Transform_and_Clip_Text (pptr, tmptext)) { 
	  CheckColour (GC.colour,textp->col);
	  tmptext->where.z *= GC.charsize;
	  mgp_showtext(tmptext->where, textp->s, GC.zthick);
	  DeleteText(tmptext);
	}
	break;

      case POLYGON:		/* polygons */

	polyp = (PolyPrim *)pptr->data;
	tmppoly = DupPolygon (polyp);

	if (Transform_and_Clip_Poly (pptr, tmppoly)) { 
	  CheckColour (GC.colour,polyp->col);     /* HERE CALC COLOUR! */
	  mgp_polygon_begin();
	  mgp_polyline (tmppoly->nvert, 0, tmppoly->data);
	  mgp_polygon_end();
	  DeletePolygon (tmppoly);
	}
	break;

      case SPHERE:

	prompt ("spheres not yet implemented\n");
	break;

      default:
	prompt ("cannot handle primitive\n");
      }

      pptr = pptr->next;	/* next primitive */
    }
    mgp_endobj();		/* end visible object */
  } 


/* Now traverse list of children with grandchildren etc. */


  iptr = optr->inst_head;	/* first object in instance list */

  while (iptr != NULL) {

    if (iptr->trf != NULL) {	/* if we have an image transformation, */
      pushmat(&MTrf);		/* apply it. */
      {
	Matrix4 M;
	m4_copy (&MTrf, &M);
	m3_matmul (iptr->trf, &M, &MTrf);
      }

      if (Debug)
	m4_print ("Applying instance transformation:",&MTrf);
      }

    CopyGC(iptr->GC, iptr->obj->GC);

    if (iptr->obj != NULL) {
      DisplayObject (iptr->obj);
    }

    if (iptr->trf != NULL) {	/* Restore local transformation */
      popmat(&MTrf);
    }
    iptr = iptr->next;		/* do next object in instance list */
  }

}

/*
  TraverseObject -- Traverse the given object and its children.
  mok 980527 @@
*/
Object *TraverseObject (optr)
     Object *optr;
{
  Instance *iptr;
  Primitive *pptr;
  ObjViewList *viewl;
  LinePrim *linep;
  TextPrim *textp;
  PolyPrim *polyp;
  Point3 *vertex;
  int i;

  if (!optr) {
    prompts ("Object not found\n");
    return NULL;
  }

  printf ("\nbegin %s\n",optr->name);

  CopyGC (optr->GC, &GC);

  viewl = optr->viewlist;

  if (viewl) {
    while (viewl) {
      switch (viewl->type) {
      case 1:			/* View reference point */
	printf ("view_reference_point %.3f %.3f %.3f\n",
		viewl->data.vrp.x, viewl->data.vrp.y, viewl->data.vrp.z); 
	break;
      case 2:			/* View plane normal */
	printf ("view_point_normal %.3f %.3f %.3f\n",
		viewl->data.vpn.x, viewl->data.vpn.y, viewl->data.vpn.z); 
	break;
      case 3:			/* Direction of projection (parallel) */
	printf ("parallel %.3f %.3f %.3f\n",
	   viewl->data.dop.x, viewl->data.dop.y, viewl->data.dop.z);
	break;
      case 4:			/* Center of projection (Perspective) */
	printf ("perspective %.3f %.3f %.3f\n",
	   viewl->data.cop.x, viewl->data.cop.y, viewl->data.cop.z); 
	break;
      case 5:			/* View up direction */
	printf ("up_direction %.3f %.3f %.3f\n",
		viewl->data.vup.x, viewl->data.vup.y, viewl->data.vup.z); 
	break;
      case 6:			/* View plane window */
	printf ("window %.3f %.3f %.3f %.3f\n", 
		viewl->data.window.min.x, viewl->data.window.max.x,
		viewl->data.window.min.y, viewl->data.window.max.y);
	break;
      case 7:			/* Viewport */
	printf ("viewport %.3f %.3f %.3f %.3f\n", 
		viewl->data.viewport.min.x, viewl->data.viewport.max.x,
		viewl->data.viewport.min.y, viewl->data.viewport.max.y);
	break;
      case 8:			/* Device viewport */
	printf ("zthick %.2f %.2f\n", 
		viewl->data.device.min.z, viewl->data.device.max.z);
	break;
      case 9:			/* Viewing mode (Parallel or perspective) */
	/*
	switch (viewl->data.ViewMode) {
	case PARALLEL:
	  printf ("parallel\n");
	  break;
	case PERSPECTIVE:
	  printf ("perspective\n");
	  break;
	}
	*/
	break;
      case 10:			/* Clip flag */
	printf ("clipflag %d\n", viewl->data.ClipFlag);
	break;
      case 11:			/* Front and back clipping limits */
	printf ("slab %.2f %.2f\n", viewl->data.Clip.x, viewl->data.Clip.y);
	break;
      }
      viewl = viewl->next;
    } 
  }

  if (optr->visible) {		/* visibility test */
    pptr = optr->prim_head;

    while (pptr != NULL) {
      switch (pptr->type) {
      case LINESEGMENT:
	linep = (LinePrim *)pptr->data;

	if (linep->col.r != GC.colour.r || linep->col.g != GC.colour.g || 
	    linep->col.b != GC.colour.b ) {
	  printf ("colour %.2f %.2f %.2f\n", 
		  linep->col.r, linep->col.g, linep->col.b);
	  GC.colour = linep->col;
	}

	if (linep->type != GC.linetype) {
	  switch (linep->type) {
	  case SOLID:
	    printf ("line_type solid\n"); break;
	  case DASHED:
	    printf ("line_type dashed\n"); break;
	  case DOTTED: 
	    printf ("line_type dotted\n"); break;
	  case DASHDOT:
	    printf ("line_type dash_dot\n"); break;
	  }
	  GC.linetype = linep->type;
	}
	
	printf ("move %.3f %.3f %.3f\n", 
		linep->from.x, linep->from.y, linep->from.z);
	printf ("line %.3f %.3f %.3f\n",
		linep->to.x, linep->to.y, linep->to.z);
	break;
 
      case TEXTSEGMENT:
	textp = (TextPrim *)pptr->data;

	if (textp->col.r != GC.colour.r || textp->col.g != GC.colour.g || 
	    textp->col.b != GC.colour.b ) {
	  printf ("colour %.3f %.3f %.3f\n", textp->col.r, 
		textp->col.g, textp->col.b);
	  GC.colour = textp->col;
	}

	if (textp->size != GC.charsize) {
	  printf ("text_size %.2f\n", textp->size);
	  GC.charsize = textp->size;
	}

	printf ("text %.3f %.3f %.3f \"%s\"\n", 
		textp->where.x, textp->where.y, textp->where.z, textp->s);
	break;

      case POLYGON:
	polyp = (PolyPrim *)pptr->data;

	if (polyp->col.r != GC.colour.r || polyp->col.g != GC.colour.g || 
	    polyp->col.b != GC.colour.b ) {
	  printf ("colour %.2f %.2f %.2f\n", polyp->col.r, 
		  polyp->col.g, polyp->col.b);
	  GC.colour = polyp->col;
	}

	vertex = polyp->data;
	printf ("polygon\n");
	for (i=0; i< polyp->nvert; i++) {
	  printf ("%.3f %.3f %.3f\n",
		vertex->x, vertex->y, vertex->z);
	  vertex++;
	}
	break;
	
      case SPHERE:
	break;

      default:
	prompts ("cannot handle primitive\n");
      }

      pptr = pptr->next;	/* next primitive */
    }
  } 


  /* Now traverse list of instances */
  iptr = optr->inst_head;	/* first object in instance list */

  if (!iptr) {
    printf ("end_object # no instances \n");
    return NULL;
  }

  if (iptr->trf) {		/* do we have an instance transformation? */
    register i;
    printf ("transform\n");	/* output the transpose :-( */
    for (i=0; i<3; i++) {
      printf ("  %10.5f %10.5f %10.5f\n", iptr->trf->element[0][i], 
	      iptr->trf->element[1][i], iptr->trf->element[2][i]);
    }
    printf ("translate %.3f %.3f %.3f\n", iptr->trf->element[3][0],
	    iptr->trf->element[3][1], iptr->trf->element[3][2]);
  }

  while (iptr) {
    printf ("instance %s\n", iptr->obj->name);
    iptr = iptr->next;		/* do next object in instance list */
  }
  printf ("end_object\n");

  /* Descend into list of instances */

  iptr = optr->inst_head;
  while (iptr) {
    CopyGC (iptr->GC, iptr->obj->GC);
    if (iptr->obj)
      TraverseObject(iptr->obj);
    else
      return NULL;
    iptr = iptr->next;		/* do next object in instance list */
  }
  return NULL;
}


/****** DISPLAYLIST ROUTINES *******/



/*
  D i s p l a y D i s p l a y L i s t
  Output the displaylist
  930215 mok.
*/
Object *DisplayDisplayList()
{
  Primitive *pptr;
  LinePrim *linep;
  TextPrim *textp;
  PolyPrim *polyp;
  Point3 P1, P2;

  if (DisplayList == NULL) {
    prompts ("Object not found\n");
    return NULL;
  }

  if (Verbose)
    prompt ("Displaying DisplayList\n");

  mgp_linetype (GC.linetype);
  mgp_setcol (0,GC.colour.r, GC.colour.g, GC.colour.b);
  mgp_textsize (GC.charsize);

  pptr = DisplayList->prim_head;
  mgp_beginobj(DisplayList->name);

  while (pptr != NULL) {
      
    switch (pptr->type) {
	
    case LINESEGMENT:		/* Line Segments */

      linep = (LinePrim *)pptr->data;

      P1.x = linep->from.x;  P1.y = linep->from.y; P1.z = linep->from.z;
      P2.x = linep->to.x;    P2.y = linep->to.y;   P2.z = linep->to.z;

      if (linep->type != GC.linetype) {
	mgp_linetype (linep->type);
	GC.linetype = linep->type;
      }
      CheckColour (GC.colour,linep->col); 
      mgp_moveto (&P1);
      mgp_lineto (&P2, GC.zthick);
      break;
 
    case TEXTSEGMENT:		/* Text segments */
      textp = (TextPrim *)pptr->data;
      if (!GC.zthick) {
	if (textp->size != GC.charsize) {
	  mgp_textsize (textp->size);
	  GC.charsize = textp->size;
	}
      }
      
      P1.x = textp->where.x;  P1.y = textp->where.y; P1.z = textp->where.z;


      CheckColour (GC.colour,textp->col);
      P1.z *= GC.charsize;
      mgp_showtext(&P1, textp->s, GC.zthick);
      break;

    case POLYGON:
      {
	Colour newcol;
	Surface surf;
	Vector3 V;

	polyp = (PolyPrim *)pptr->data;

	if (view_mode == PARALLEL) {
	  v3_copy (&dop, &V);
	} 
	else {
	  v3_sub (&polyp->centre, &cop, &V);
	}

	if (Shading != 0) {
	  v3_normalize (&V);

	  GC.surf.ambient = .10;
	  GC.surf.diffuse = 1.0;
	  GC.surf.specular = 1.0;
	  GC.surf.pow = 70.0;

	  Lights[0].intensity = 3.0;
	  Lights[0].position.x = 0.5;
	  Lights[0].position.y = 0.5;
	  Lights[0].position.z = -1.0;

	  Lights[0].col.r = 1.0;
	  Lights[0].col.g = 1.0;
	  Lights[0].col.b = 1.0;

	  Lights[0].atten[0] = 1.0;
	  Lights[0].atten[1] = 2.0;
	  Lights[0].type = POSITIONAL;

	  NumberLights = 1;

	  PolyShade (&polyp->col, &polyp->centre, &polyp->normal.vec, &V,
		     &GC.surf, NumberLights, Lights, &newcol);
	}
	else
	  StructCopy (polyp->col, newcol); 

      CheckColour (GC.colour,newcol);
/* 	mgp_colour (&newcol); */
	mgp_polygon_begin();
	mgp_polyline (polyp->nvert, 0, polyp->data);
	mgp_polygon_end();
      }
      break;

    case SPHERE:
      prompt ("Spheres not yet implemented\n");
      break;

    default:
      prompt ("Cannot handle primitive\n");
    }

    pptr = pptr->next;	/* next primitive */
  }
  mgp_endobj();
}

/*
  A d d T o D i s p l a y L i s t
  Traverse a given object and its children, adding to the display list object.
  930309 mok.
*/
static void AddToDisplayList (optr)
     Object *optr;
{
  Instance *iptr;
  Primitive *pptr, *primnew;
  LinePrim *linep;
  TextPrim *textp;
  PolyPrim *polyp;
  Point3 P1, P2, *Pp1, *Pp2;
  int i;

  if (optr == NULL) {
    prompt ("Object not found\n");
    return;
  }

/* Set the objects viewing parameters */

  SetViewPar (optr, &optr->viewpar);
  if (Verbose) {
    fprintf (stderr, "Viewing parameter block for %s\n", optr->name);
    printViewPar (&optr->viewpar);
  }

  CopyGC (optr->GC, &GC);

  BldViewingTransform(&optr->viewpar, &CurrentView);
  BldViewportMapping (&optr->viewpar, &CurrentView);
  BldDeviceMapping   (&optr->viewpar, &CurrentView);

  if (Verbose) {
    prompts ("Adding to displaylist ",optr->name);
    printview (&CurrentView);
  }

  if (optr->visible) {		/* visibility test */
    pptr = optr->prim_head;

    while (pptr != NULL) {
      
      switch (pptr->type) {
	
      case LINESEGMENT:		/* Line Segments */

	linep = DupLine ((LinePrim *)pptr->data);

	if (Transform_and_Clip_Line (pptr, linep)) {
	  primnew = CreatePrimitive(DisplayList);
	  primnew->data = (void *)linep;
	  primnew->type = LINESEGMENT;
	  primnew->zmin = pptr->zmin;
	}
	else
	  DeleteLine(linep); 
	
	break;
 
      case TEXTSEGMENT:		/* Text segments */

	textp = DupText ((TextPrim *)pptr->data);
	if (Transform_and_Clip_Text (pptr, textp)) {
	  primnew = CreatePrimitive(DisplayList);
	  primnew->data = (void *)textp;
	  primnew->type = TEXTSEGMENT;
	  primnew->zmin = pptr->zmin;
	}
	else
	  DeleteText(textp);

	break;

      case POLYGON:

	/* Make a copy of the polygon to work on */
	polyp = DupPolygon((PolyPrim *)pptr->data);

	if (Transform_and_Clip_Poly (pptr, polyp)) {
	  primnew = CreatePrimitive(DisplayList);
	  primnew->data = (void *) polyp;
	  primnew->type = POLYGON;
	  primnew->zmin = pptr->zmin; 
	/*   fprint (stderr,"POLY zmin = %f\n", pptr->zmin); /* DBG */
	}
	else			/* If completely clipped, remove again */
	  DeletePolygon(polyp);

	break;

      case SPHERE:
	prompt ("Spheres not yet implemented\n");
	break;

      default:
	prompt ("Cannot handle primitive\n");
      }

      pptr = pptr->next;	/* next primitive */
    }
  }				/* end visible object */


/* Now traverse list of children with grandchildren etc. */


  iptr = optr->inst_head;	/* first object in instance list */

  while (iptr != NULL) {
				/* Copy objects' viewpar to child object */
    bcopy (&optr->viewpar, &iptr->obj->viewpar, sizeof(ViewPar));
				/* Copy instance's GC to child object */
    CopyGC(iptr->GC, iptr->obj->GC);

    if (iptr->trf != NULL) {	/* if we have an image transformation, */
      pushmat(&MTrf);		/* apply it. */
      {
	Matrix4 M;
	m4_copy (&MTrf, &M);
	m3_matmul (iptr->trf, &M, &MTrf);
      }

      if (Debug) {
	m4_print ("Applying instance transformation:",&MTrf);
      }
    }

    AddToDisplayList (iptr->obj);

    if (iptr->trf != NULL) {	/* Restore local transformation */
      popmat(&MTrf);
    }
    iptr = iptr->next;		/* do next object in instance list */
  }

}

/*
  D e p t h C o m p a r e
  Return compare depth of primitives.
*/
static int DepthCompare (p, q)
     Primitive *p, *q;
{

  if (p->zmin > q->zmin)
    return -1;
  else if (p->zmin < q->zmin)
    return 1;
  else
    return 0;
}

/*
  C r e a t e D i s p l a y L i s t 
  Create a display list from the given object. The diplaylist is *not* linked
  into our normal topobject structure.
  mok 930309
*/
static void CreateDisplayList (obj)
     Object *obj;
{
  int exists;
  extern Matrix4 *m4_copy();
  extern char *strsave();

  exists = 0;

  if (DisplayList != NULL) {
    exists = 1;
    DeleteObject (DisplayList);	/* if object exists already, zap it*/
    if (Verbose)
      prompt ("Overwriting displaylist\n");
  } else {
    DisplayList = NEWSTRUCT(Object);	/* no display list object existed */
    if (Verbose) 
      prompt ("Creating displaylist\n");
  }

  DisplayList->name = strsave("DisplayList");
  DisplayList->visible = 1;		/* default visibility on */
  DisplayList->viewlist = NULL;

  ResetViewPar (&DisplayList->viewpar);

  if (!exists) {
    DisplayList->next = NULL;	   /* pointer to next object in list */
    DisplayList->inst_head = NULL; /* pointer to list of instances */
    DisplayList->inst_tail = NULL; /* pointer to last in list of instances */
    DisplayList->prim_head = NULL; /* pointer to first primitive of this obj */
    DisplayList->prim_tail = NULL; /* pointer to last primitive of this obj */
  }

  /* Make sure our top object has the viewing parameters set */
  ResetViewPar (&obj->viewpar);

  /* Kludge for backward compat: Set any viewing parameters defined 
     outside open objects */
  SetViewPar (RootObject, &obj->viewpar); /* kludge */
  
  AddToDisplayList (obj);

/* DBG 
  { Primitive *p;
    p = DisplayList->prim_head;
    while (p) {
      fprintf (stderr, "TYPE = %d, ZMIN = %f\n", p->type, p->zmin);
	p = p->next;
    }
  }
 DBG END */

  lqsort(&DisplayList->prim_head, DepthCompare);

/* DBG 
  { Primitive *p;
    fprintf (stderr, "*****************\n");
    p = DisplayList->prim_head;
    while (p) {
      fprintf (stderr, "TYPE = %d, ZMIN = %f\n", p->type, p->zmin);
	p = p->next;
    }
  }
 DBG END */


}


/****** USER CALLABLE OBJECT ROUTINES *******/

/*
  r e n a m e _ o b j e c t
  Rename a given object. Otherwise don't change the contents of the object.
  mok 930218.
*/
void rename_object (old, new)
     char *old, *new;
{
  Object *optr;
  extern char *strsave();
  
  if ((optr = FindObject (old)) == NULL) {
    prompts ("Cannot rename non-existing object: ", old);
    return;
  }
  
  free (optr->name);
  optr->name = strsave (new);

}


/*
  d e l e t e _ o b j e c t
*/
void delete_object (name)
     char *name;
{
  if ((DeleteObject (FindObject (name))) == NULL) 
    prompts ("Cannot delete non-existing object: ",name);
}


/*
  d i s p l a y _ o b j e c t
  Display the given object
  mok 930219.
*/
void display_object(name)
     char *name;
{
  BuildViewingTransform();
  BuildViewportMapping();
  BuildDeviceMapping();


  switch (Hidden) {
  case 0:			/* nothing */
    if (!PlotInit)
      activate_plot();
    DisplayObject(FindObject(name)); /* find a pointer to the object */
    break;

  case 1:			/* depth sorting */
    if (!PlotInit)
      activate_plot();
    CreateDisplayList (FindObject(name));
    DisplayDisplayList();
    break;

  case 2:			/* traverse structure for debugging  */
    TraverseObject (FindObject(name));
    printf ("display %s\n", name);
    break;

  default:
    prompt ("Unknown mode for hidden line elimination\n");
  }
}

#ifdef DEBUG
/*
  t r a v e r s e _ s t r u c t u r e
  Traverse the object structure from the top and write out the contents.
  930205 mok. For debugging purposes.
*/
void traverse_structure()
{
  Object *optr;

  optr = TopObject;

  while (optr != NULL) {
    display_object (optr->name);
    optr = optr->next;          /* next object */
  }
}
#endif


/****** DRAWING ROUTINES *******/


/*
  l i n e _ a b s _ 3
  Output a line from current point to new point. 
  mok 930121.
*/

void line_abs_3 (x,y,z)
     float x,y,z;
{
  Point3 X1, X2;

  X1.x = CP.x; X1.y = CP.y; X1.z = CP.z;
  X2.x = x; X2.y = y; X2.z = z;
  AddLine (CurrentObject, &X1, &X2);
  CP.x = x; CP.y = y; CP.z = z;
}

/*

  m o v e _ a b s _ 3
  Output a line from current point to new point.
  mok 930120.
*/

void move_abs_3 (x,y,z)
     float x,y,z;
{
  CP.x = x; CP.y = y; CP.z = z;
}


/*
  l i n e _ r e l _ 3
  Output a line from current point to new point. 
  mok 930120.
*/

void line_rel_3 (dx,dy,dz)
     float dx,dy,dz;
{
  Point3 X1, X2;

  X1.x = CP.x; X1.y = CP.y; X1.z = CP.z;
  X2.x = CP.x + dx; X2.y = CP.y + dy; X2.z = CP.z + dz; 
  AddLine (CurrentObject, &X1, &X2);
  CP.x += dx; CP.y += dy; CP.z += dz;

}

/*

  m o v e _ r e l _ 3
  Output a line from current point to new point
  mok 930120
*/

void move_rel_3 (dx,dy,dz)
     float dx,dy,dz;
{
  CP.x += dx; CP.y += dy; CP.z += dz; 
}

/*
  l i n e _ a b s _ 2
  Output a line from current point to new point. 
  mok 930120
*/

void line_abs_2 (x,y)
     float x,y;
{
  Point3 X1, X2;

  X1.x = CP.x; X1.y = CP.y; X1.z = CP.z;
  X2.x = x; X2.y = y; X2.z = CP.z;
  AddLine (CurrentObject, &X1, &X2);
  CP.x = x; CP.y = y; 
}

/*

  m o v e _ a b s _ 2
  Output a line from current point to new point
  mok 930121
*/

void move_abs_2 (x,y)
     float x,y;
{
  CP.x = x; CP.y = y;
}


/*
  l i n e _ r e l _ 2
  Output a line from current point to new point
  mok 930121
*/

void line_rel_2 (dx,dy)
     float dx,dy;
{
  Point3 X1, X2;

  X1.x = CP.x; X1.y = CP.y; X1.z = CP.z;
  X2.x = CP.x + dx; X2.y = CP.y + dy; X2.z = CP.z;
  AddLine (CurrentObject, &X1, &X2);
  CP.x += dx; CP.y += dy; 
}

/*
  m o v e _ r e l _ 2
  Output a line from current point to new point
  mok 930121
*/

void move_rel_2 (dx,dy)
     float dx,dy;
{
  CP.x += dx; CP.y += dy;
}

/*
  p o l y g o n _ 3 
  pDefine a polygon
  mok 930219
*/

void polygon_3 (ct, points)
     int ct;
     Vector3 points[];
{
  AddPolygon (CurrentObject, ct, points);
}

/*
  t e x t 
  Output textstring at current position.
  mok 930122.
*/
void text (str)
     char *str;
{
  Point3 X;

  v3_set (&X,CP.x,CP.y,CP.z);
  AddText (CurrentObject, &X, str);
}

/*
  t e x t _ 3 
  Output textstring at specified position. Sets CP
  mok 930122.
*/
void text_3 (x,y,z,str)
     float x,y,z;
     char *str;
{
  Point3 X;

  CP.x = x; CP.y = y; CP.z = z;
  v3_set (&X,CP.x,CP.y,CP.z);
  AddText (CurrentObject, &X, str);
}

/*
  t e x t _ 2
  Output textstring at specified position. Sets CP.
  mok 930122.
*/
void text_2 (x,y,str)
     float x,y;
     char *str;
{
  Point3 X;

  CP.x = x; CP.y = y; 
  v3_set (&X,CP.x,CP.y,CP.z);
  AddText (CurrentObject, &X, str);
}

/*
  c r e a t e _ i n s t a n c e
  Create an intance in the current object
  mok 930216
*/
void create_instance (objnam)
     char *objnam;
{

  if (Verbose) {
    char buf[80];
    sprintf (buf,"Creating instance of object %s\n",objnam); 
    prompt (buf);
  }

  if (CurrentObject != NULL)  {
    CreateInstance (CurrentObject, objnam, &MTrf, &GC);
  }
  else
    prompt ("No open object\n");
}


/*
  c r e a t e _ o b j e c t
  Start the plotting of a 'segment'.
  mok 930120
*/
void create_object (name)
     char *name;
{
  pushmat (&MTrf);		/* push current local transform */
  CurrentObject = CreateNamedObject(name);
}

/*
  c l o s e _ o b j e c t
  Close the object. 
  mok 930120
*/
close_object ()
{
  popmat (&MTrf);		/* restore previous local transform */
  if (Verbose)
    printObjViewList (CurrentObject);
  CurrentObject = NULL;		/* no current object */
}


/*
  c h e c k d a t a
  Print out some of the vectors & matrices.
*/

void checkdata ()
{
  v3_print ("Window low corner:", &window_lo);
  v3_print ("Window hi corner:", &window_hi);
  v3_print ("Viewport3 low corner:", &viewport3_lo);
  v3_print ("Viewport3 hi corner:", &viewport3_hi);
  v3_print ("View reference point:", &vrp);
  v3_print ("View point normal:", &vpn);
  v3_print ("Direction of projection:", &dop);
  v3_print ("Center of projection:", &cop);
  v3_print ("View up direction:", &vup);
  m4_print ("Translation matrix T:", &T);
  m4_print ("Rotation matrix R:", &R);
  m4_print ("Invert z Trl:", &Trl);
  m4_print ("Shear matrix SHz:", &SHz);
  m4_print ("Scaling matrix Spar:", &Spar);
  m4_print ("Translation matrix Tpar:", &Tpar);
  m4_print ("Transformation matrix Npar:", &CurrentView.Np);
  m4_print ("Unit cube to viewport transformation matrix Ipar:",
	   &CurrentView.Ip);
  m4_print ("Nper:", &CurrentView.Np);
  m4_print ("Sper:", &Sper);
  m4_print ("Tper:", &Tper);
  m4_print ("Iper:", &CurrentView.Ip);
  m4_print ("NDC to physical device transformation matrix N2D:",
	   &CurrentView.N2D);
  m4_print ("Modelling transform MTrf:", &MTrf);
/*
  m4_print ("Modelling rotation matrix Mrot:", &Mrot);
  m4_print ("Modelling scale matrix Msca:", &Msca);
  v3_print ("Modelling translation vector Mtra:", &Mtra);
*/
  fprintf (stderr, "Back = %f,  Front = %f\n",Back,Front);
}

/* end of $Source$ */
