/* 
 * $Id: mgpbuild.c 39 2007-10-22 14:47:41Z mok $
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

#include "stdio.h"
#include "mgp.h"
#include "mgpdefs.h"

/***** BUILD ROUTINES ******/


/* 
   B u i l d V i e w i n g T r a n s f o r m 
   Calculate the 4x4 matrix describing the normalizing transformation, that
   maps the world coordinates into the unit cube.
   mok 930120, mok 931121
*/

void BldViewingTransform(viewpar, view)
     ViewPar *viewpar;
     View *view;
{
  Vector3 rx, ry, rz, dop1, vrp1, cw;
  Matrix4 M4;
  Matrix4 R, Tper, Trl, SHz, Sper, T, Tpar, Spar;
  Matrix4 *m4_ident();

  /* Define R matrix. Common to parallel & perspective projections */

  m4_ident (&R);  m4_ident (&Tper);  m4_ident (&SHz);  m4_ident(&Sper);
  m4_ident (&T);  m4_ident (&Tpar);  m4_ident (&Spar); 
  m4_ident (&Trl); Trl.element[2][2] = -1.0;
 
  rz.x = -viewpar->vpn.x ; rz.y = -viewpar->vpn.y ; rz.z = -viewpar->vpn.z;
  v3_normalize(&rz);
  v3_cross(&viewpar->vpn, &viewpar->vup, &rx);
  v3_normalize (&rx);  
  v3_cross(&rz, &rx, &ry);
  R.element[0][0] = rx.x; R.element[0][1] = ry.x;  R.element[0][2] = rz.x;
  R.element[1][0] = rx.y; R.element[1][1] = ry.y;  R.element[1][2] = rz.y; 
  R.element[2][0] = rx.z; R.element[2][1] = ry.z;  R.element[2][2] = rz.z;

  if (viewpar->ViewMode == PERSPECTIVE) {

    /* Define Tper */
    Tper.element[3][0] = -(viewpar->vrp.x + viewpar->cop.x);
    Tper.element[3][1] = -(viewpar->vrp.y + viewpar->cop.y);
    Tper.element[3][2] = -(viewpar->vrp.z + viewpar->cop.z);

    /* Determine VRP' */
    m3_matmul (&Tper, &R, &M4);
    m3_matmul (&M4, &Trl, &view->Np);
    v3_mulPointByProjMatrix (&viewpar->vrp, &view->Np, &vrp1);

    /* Determine center-of-window CW */
    cw.x = vrp1.x + 0.5*(viewpar->window.min.x + viewpar->window.max.x);
    cw.y = vrp1.y + 0.5*(viewpar->window.min.y + viewpar->window.max.y);
    cw.z = vrp1.z;

    /* Now define shearing matrix SHz */
    SHz.element[2][0] = -cw.x/cw.z;
    SHz.element[2][1] = -cw.y/cw.z;
    
    /* Define Sper, that scales to canonical volume */

    {
      double d;

      d = vrp1.z + viewpar->Clip.y; /* back */
       Sper.element[0][0] = 
	2.0*vrp1.z/( (viewpar->window.max.x - viewpar->window.min.x) * d );
      Sper.element[1][1] = 
	2.0*vrp1.z/((viewpar->window.max.y-viewpar->window.min.y)* d );
      Sper.element[2][2] = 1.0/d;
    }

    /* Finally apply shear & scale to Nper: */
    m3_matmul (&view->Np, &SHz, &M4);
    m3_matmul (&M4, &Sper, &view->Np);

    /* F & V p.295 */
    view->PerZmin = (vrp1.z+ viewpar->Clip.x)/(vrp1.z+viewpar->Clip.y);
/*    PerZproj = vrp1.z/(vrp1.z + viewpar->Clip.y);  (back)*/

  } else if (viewpar->ViewMode == PARALLEL) {

    /* 1. Define T matrix translates vrp to origin: */
    T.element[3][0] = -viewpar->vrp.x;
    T.element[3][1] = -viewpar->vrp.y;
    T.element[3][2] = -viewpar->vrp.z;
  
    /* 5. Build shearing matrix SHz */
    m3_matmul (&R, &Trl, &M4);
    v3_mulPointByProjMatrix (&viewpar->dop, &M4, &dop1);
    SHz.element[2][0] = -dop1.x/dop1.z;
    SHz.element[2][1] = -dop1.y/dop1.z;

    /* 6. Transform sheared volume into unit cube */

    Tpar.element[3][0] = -viewpar->window.min.x; 
    Tpar.element[3][1] = -viewpar->window.min.y;
    Tpar.element[3][2] = -viewpar->Clip.x; /* front */

    Spar.element[0][0] = 1.0/(viewpar->window.max.x - viewpar->window.min.x);
    Spar.element[1][1] = 1.0/(viewpar->window.max.y - viewpar->window.min.y);
    Spar.element[2][2] = 1.0/(viewpar->Clip.y - viewpar->Clip.x);

    m3_matmul (&T, &R, &view->Np);
    m3_matmul (&view->Np, &Trl, &M4);
    m3_matmul (&M4, &SHz, &view->Np);
    m3_matmul (&view->Np, &Tpar, &M4);
    m3_matmul (&M4, &Spar, &view->Np);
  }
  else
    fprintf (stderr, "BuildViewingTransform: unknown mode %d\n",
	     viewpar->ViewMode);

/*  printM4 ("Np set:",&view->Np); */

}



/*
  B u i l d V i e w p o r t M a p p i n g
  Create the 4x4 matrix that maps the unit cube coordinates to the
  3D viewport (NDC coordinates).
  mok 930121, mok 931121
*/

void BldViewportMapping(viewpar,view)
     ViewPar *viewpar;
     View *view;
{ 

  if (viewpar->ViewMode == PERSPECTIVE) {
    Matrix4 M, Ta, Sa, T1, tmp, *m4_ident();

    m4_ident(&M); m4_ident(&Ta); m4_ident(&Sa); m4_ident(&T1);
    M.element[2][2] = 1.0/(1.0-viewpar->viewport.min.z);
    M.element[3][2] = -viewpar->viewport.min.z/(1.0-viewpar->viewport.min.z);
    M.element[2][3] = 1.0;
    M.element[3][3] = 0.0;
    Ta.element[3][0] = 1.0; Ta.element[3][1] = 1.0;
    Sa.element[0][0] = 0.5*(viewpar->viewport.max.x - viewpar->viewport.min.x);
    Sa.element[1][1] = 0.5*(viewpar->viewport.max.y - viewpar->viewport.min.y);
    Sa.element[2][2] = viewpar->viewport.max.z - viewpar->viewport.min.z;
    T1.element[3][0] = viewpar->viewport.min.x;
    T1.element[3][1] = viewpar->viewport.min.y;
    T1.element[3][2] = viewpar->viewport.min.z;

    /* Now form Iper = M.Ta.Sa.T1 (p. 302) */
    m3_matmul (&M, &Ta, &view->Ip);
    m3_matmul (&view->Ip, &Sa, &tmp);
    m3_matmul (&tmp, &T1, &view->Ip);

  } else if (viewpar->ViewMode == PARALLEL) {

    m4_ident (&view->Ip);
    view->Ip.element[0][0] = 
      (viewpar->viewport.max.x - viewpar->viewport.min.x);
    view->Ip.element[1][1] = 
      (viewpar->viewport.max.y - viewpar->viewport.min.y);
    view->Ip.element[2][2] = 
      (viewpar->viewport.max.z - viewpar->viewport.min.z);
    view->Ip.element[3][0] = viewpar->viewport.min.x;
    view->Ip.element[3][1] = viewpar->viewport.min.y;
    view->Ip.element[3][2] = viewpar->viewport.min.z;
  } 
  else 
    fprintf (stderr,"BuildViewportMapping: unknown mode %d\n", 
	     viewpar->ViewMode);

/*  printM4 ("Ip set:",&view->Ip); */

}

/*
  B u i l d D e v i c e M a p p i n g
  Build the transformation matrix that takes us from normalized device 
  coordinates to device coordinates.
  mok 930120, mok 931121
*/
void BldDeviceMapping (viewpar, view)
     ViewPar *viewpar;
     View *view;
{
  float xmin, xmax, ymin, ymax, zfar, znear;
  float mgp_getplotaspect();

  /* Ask device driver for size of paper (SHOULD GO IN INIT ROUTINE!!)*/
  mgp_getplotsize (&xmin, &xmax, &ymin, &ymax, &zfar, &znear);

  if (mgp_getplotaspect() > 1.0) /* define a square plotting area */
    ymax = ymin + (xmax-xmin);
  else
    xmax = xmax + (ymax-ymin);

  viewpar->device.min.x = xmin;
  viewpar->device.max.x = xmax;
  viewpar->device.min.y = ymin;
  viewpar->device.max.y = ymax;

  view->N2D.element[0][0] = (viewpar->device.max.x - viewpar->device.min.x);
  view->N2D.element[1][1] = (viewpar->device.max.y - viewpar->device.min.y);
  view->N2D.element[2][2] = (viewpar->device.max.z - viewpar->device.min.z);
  view->N2D.element[3][0] = viewpar->device.min.x;
  view->N2D.element[3][1] = viewpar->device.min.y;
  view->N2D.element[3][2] = viewpar->device.min.z;

}

/*** end of $Source$ ***/
