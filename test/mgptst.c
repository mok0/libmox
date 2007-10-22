/* $Id:$ */

#include <stdio.h>
#include "ggveclib.h"
#include "mgp.h"
#include "mgpdefs.h"

/* h o u s e -- Draw house */

void house() {

  move_abs_3 (0.0,0.0,30.0);
  line_rel_2 (16.0,0.0);
  line_rel_2 (0.0,10.0);
  line_rel_2 (-8.0,6.0);
  line_rel_2 (-8.0,-6.0);
  line_rel_2 (0.0,-10.0);

/* draw face in z = 54 plane */
  move_abs_3 (0.0,0.0,54.0);
  line_rel_2 (16.0,0.0);
  line_rel_2 (0.0,10.0);
  line_rel_2 (-8.0,6.0);
  line_rel_2 (-8.0,-6.0);
  line_rel_2 (0.0,-10.0);

/* Now connect front and read faces. */

  move_abs_3 (0.0,0.0,30.0);
  text ("a");
  line_rel_3 (0.0,0.0,24.0);
  text ("b");
  move_abs_3 (16.0,0.0,30.0);
  text ("c");
  line_rel_3 (0.0,0.0,24.0);
  text ("d");
  move_abs_3 (16.0,10.0,30.0);
  text ("e");
  line_rel_3 (0.0,0.0,24.0);
  text ("f");
  move_abs_3 (8.0,16.0,30.0);
  text ("g");
  line_rel_3 (0.0,0.0,24.0);
  text ("h");
  move_abs_3 (0.0,10.0,30.0);
  text ("i");
  line_rel_3 (0.0,0.0,24.0);
  text ("j");
}

/****  main  ****/

main ()
{
  float x,y,d;
  int i;
  Matrix4 m;

  M4ident (&m);
  m.element[0][0] = 0.;
  m.element[0][1] = 1.;
  m.element[1][1] = 0;
  m.element[1][0] = 1.;

/* Figure 8.46 */
  prompt ("Figure 8.46\n");
  set_projection (PERSPECTIVE, 8., 6., 84.);
  set_window (-50.,50.,-50.,50.);
  create_object ("A");
  house ();
  close_object ();
  sleep(1);

/* Figure 8.47 */
  prompt ("Figure 8.47\n");
  set_view_reference_point (0.,0.,54.);
  set_projection (PERSPECTIVE, 8.,6.,30.);
  set_window (-1.,17.,-1.,17.);
  create_object ("B");
  house ();
  close_object ();
  sleep(1);

  set_view_reference_point (8.,6.,54.);
  set_projection (PERSPECTIVE, 0.,0.,30.);
  set_window (-9.,9.,-7.,11.);
  create_object ("C");
  house ();
  close_object ();
  fflush(stdout);
  sleep(1);

/* Figure 8.49 */
  prompt ("Figure 8.49\n");
  set_view_reference_point (16.,0.,54.);
  set_projection (PERSPECTIVE, 20.,25.,20.);
  set_window (-20.,20.,-5.,35.);
  create_object ("D");
  house ();
  close_object ();
  fflush(stdout);
  sleep(1);

/* Figure 8.41 */
  prompt ("Figure 8.41\n");
  set_view_reference_point (16.,0.,54.);
  set_view_plane_normal (-1.,0.,-1.);
  set_projection (PERSPECTIVE, 20.,25.,20.);
  set_window (-12.,13.,-2.,23.);
  create_object ("E");
  house ();
  close_object ();
  fflush(stdout);
  sleep(1);

/* ~ Figure 8.54 */
  prompt ("Figure ~8.54\n");
  for (i = 0; i < 3; i++) {
    d = (20.0+i*30.0)/1.414213;
    set_view_reference_point (16.,0.,54.);
    set_view_plane_normal (-1.,0.,-1.);
    set_projection (PERSPECTIVE, d ,25., d);
    set_window (-20.,20.,-5.,35.);
    create_object ("F");
    house ();
    close_object ();
    fflush(stdout);
    sleep(1);
  }


/* Figure 8.57 */
  prompt ("Figure 8.57\n");
  set_front_plane_clipping (1);
  set_back_plane_clipping (1);

  set_view_plane_normal (0.,0.,-1.);
/*  set_view_depth (-1.0,24.0); */
  set_view_depth (-1.0,14.0);
  set_view_reference_point (0., 0., 54.0);
  set_projection (PERSPECTIVE, 8., 6., 30.);
  set_window (-1.,17.,-1.,17.);
  create_object ("G");
  house ();
  close_object ();
  fflush(stdout);
  sleep(1);

/* Figure 8.55 */
  prompt ("Figure 8.55\n");
  reset_to_default();

  set_projection(PARALLEL,0.0,0.0,-1.0);
  set_window (-1.0,17.0,-1.0,17.0);
  create_object ("H");
  house ();
  close_object ();
  fflush(stdout);
  sleep(1);


/* Figure 8.56 */
  prompt ("Figure 8.56\n");
  set_view_reference_point (0.0,0.0,54.0);
  set_view_plane_normal (-1.0,0.0,0.0);
  set_projection(PARALLEL,-1.0,0.0,0.0);
  set_window (-1.0,25.0,-5.0,21.0);
  create_object ("I");
  house ();
  close_object ();
  fflush(stdout);
  sleep(1);

/* Top view, no figure */
  prompt ("Top view, no figure\n");
  set_view_reference_point (16.0,0.0,54.0);
  set_view_plane_normal (0.0,-1.0,0.0);
  set_view_up (-1.0,0.0,0.0);
  set_projection(PARALLEL,0.0,-1.0,0.0);
  set_window (-1.0,25.0,-5.0,21.0);
  create_object ("J");
  house ();
  close_object ();
  fflush(stdout);
  sleep(1);

/* Figure 8.41 */
  prompt ("Figure 8.41\n");
  set_view_up (0.0,1.0,0.0);    /* set back to normal */

  set_view_reference_point (10.0,8.0,54.);
  set_view_plane_normal (-1.0,-1.0,-1.0);
  set_projection(PARALLEL,-1.,-1.0,-1.0);
  set_window(-15.0,25.0,-15.0,25.0);
  create_object ("K");
  house ();
  close_object ();
  fflush(stdout);
  sleep(1);

/* Cavalier projection cos(45) = sin(45) = 0.7071 */
  prompt ("Cavalier projection, no figure\n");
  reset_to_default();

  set_projection(PARALLEL,0.7071,0.7071,-1.0);
  set_window (0.0,80.0,0.0,80.0);
  create_object ("L");
  house ();
  close_object ();
  fflush(stdout);
  sleep(1);

/*  traverse_structure(); */
}


/*
  Local variables:
  mode: font-lock
  End:
*/
