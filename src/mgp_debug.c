/* 
 * $Id: debug.c 35 2006-01-25 13:23:51Z mok $
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

#include "mgp.h"
#include "stdio.h"

/* 
   p r i n t V i e w P a r
   Print out the contents of the view parameter data structure. For debugging.
   mok 931122
*/
void printViewPar (viewpar) 
     ViewPar *viewpar;
{

  v3_print ("view reference point   ", &viewpar->vrp);
  v3_print ("view plane normal      ", &viewpar->vpn);
  v3_print ("direction of projection", &viewpar->dop);
  v3_print ("Center of projection   ", &viewpar->cop);
  v3_print ("View up direction      ", &viewpar->vup);
  v2_print ("Window min             ", &viewpar->window.min);
  v2_print ("Window max             ", &viewpar->window.max);
  v3_print ("Viewport min           ", &viewpar->viewport.min);
  v3_print ("Viewport max           ", &viewpar->viewport.max);
  v3_print ("Device area min        ", &viewpar->device.min);
  v3_print ("Device area max        ", &viewpar->device.max);
  fprintf (stderr, "View mode              : %d\n", viewpar->ViewMode);
  fprintf (stderr, "Clipflag               : %d\n", viewpar->ClipFlag);
  v2_print ("Clipping, Front, back  ", &viewpar->Clip);
}

/*
  p r i n t O b j V i e w L i s t
  Print out the contents of an object viewlist
  mok 931122
*/
void printObjViewList (optr)
     Object *optr;
{
  ObjViewList *viewl;

  viewl = optr->viewlist;

  fprintf (stderr, "---- View list for object %s ----\n", optr->name);

  if (viewl == NULL) {
    prompt ("Null list\n");
  }

  while (viewl) {
    fprintf (stderr, "Tag = %d\n", viewl->type); 
    switch (viewl->type) {
    case 1:			/* View reference point */
      v3_print ("View reference point", &viewl->data.vrp);
      break;
    case 2:			/* View plane normal */
      v3_print ("View plane normal   ", &viewl->data.vpn);
      break;
    case 3:			/* Direction of projection (parallel) */
      v3_print ("Direction of projection",&viewl->data.dop);
      break;
    case 4:			/* Center of projection (Perspective) */
      v3_print ("Center of projection   ", &viewl->data.cop);
      break;
    case 5:			/* View up direction */
      v3_print ("View up direction      ", &viewl->data.vup);
      break;
    case 6:			/* View plane window */
      v2_print ("Window min             ", &viewl->data.window.min);
      v2_print ("Window max             ", &viewl->data.window.max);
      break;
    case 7:			/* Viewport */
      v3_print ("Viewport min           ", &viewl->data.viewport.min);
      v3_print ("Viewport max           ", &viewl->data.viewport.max);
      break;
    case 8:			/* Device viewport */
      v3_print ("Device area min         ", &viewl->data.device.min);
      v3_print ("Device area max         ", &viewl->data.device.max);
      break;
    case 9:			/* Viewing mode (Parallel or perspective) */
      fprintf (stderr, "View mode            :%d\n",viewl->data.ViewMode);
      break;
    case 10:			/* Clip flag */
      fprintf (stderr, "ClipFlag             :%d\n",viewl->data.ClipFlag);
      break;
    case 11:			/* Front and back clipping limits */
      v2_print ("Front, Back             ", &viewl->data.Clip);
      break;
    default:
      fprintf (stderr, "printViewObjList: no such viewing parameter: %d\n",
	       viewl->type);
    }
    viewl = viewl->next;
  }
  prompt ( "--------- End of list ---------\n");
}

/* end of file */
