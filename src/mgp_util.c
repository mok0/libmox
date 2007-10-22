
#include <stdio.h>
#include <math.h>
#include "ggveclib.h"
#include "mgp.h"
#define EPS 1.0e-12

/*
  p r o m p t 
  Dump a string to stderr.
*/
void prompt (str)
     char *str;
{
  fprintf (stderr, "%s",str);
}


/*
  p r o m p t s 
  Dump two strings to stderr. (I haven't figured out how to use varargs yet).
*/
void prompts (str, s)
     char *str, *s;
{
  fprintf (stderr, "%s %s\n", str, s);
}


/*
MATRIX STACK ROUTINES
===================
*/

typedef struct stack {		/* Typedef for matrix stack routines */
  struct stack *next;
  Matrix4 mat;
} Matrix4Stack;

static Matrix4Stack *Top = NULL;

/*
  p u s h m a t 
  Push a matrix on the stack.
  mok 930125.
*/
void pushmat (m)
     Matrix4 *m;
{
  Matrix4Stack *temp;

  temp = (Matrix4Stack *) malloc(sizeof(Matrix4Stack));
  if (temp != NULL) {
    m4_copy (m, &(temp->mat));
#ifdef DEBUG
    m4_print ("Pushed:", &(temp->mat));
#endif
    temp->next = Top;
    Top = temp;
  } else
    prompt ("pushmat: failed, can't malloc\n");
}

/*
  p o p m a t
  pop a matrix from the stack.
  mok 930125.
*/
void popmat(m)
     Matrix4 *m;
{
  Matrix4Stack *temp;

  if (Top != NULL) {
    m4_copy (&(Top->mat), m);
#ifdef DEBUG
    m4_print ("popped:", m);
#endif
    temp = Top;
    Top = Top->next;
    free (temp);
  } else
    prompt ("popmat: can't pop, stack is empty\n");
}

/*
  VIEW STACK ROUTINES
  ===================
*/

typedef struct vstack {		/* Typedef for view stack routines */
  struct vstack *next;
  View view;
} ViewStack;

static ViewStack *TopV = NULL;
int ViewCt = 0;

/*
  p u s h v i e w 
  Push viewing environment on the stack.
  mok 931115.
*/
void pushview (v)
     View *v;
{
  ViewStack *temp;

  temp = (ViewStack *) malloc(sizeof(ViewStack));
  if (temp != NULL) {
    bcopy (v, &(temp->view), sizeof(View));
    temp->next = TopV;
    TopV = temp;

    ViewCt +=1 ; fprintf (stderr, "PUSH: %d views on stack\n", ViewCt);
/* DBG */
  } else
    prompt ("pushview: failed, can't malloc\n");
}

/*
  p o p v i e w
  pop viewing environment from the stack.
  mok 931115
*/
void popview(v)
     View *v;
{
  ViewStack *temp;

  if (TopV != NULL) {
    bcopy (&(TopV->view), v, sizeof(View));
    temp = TopV;
    TopV = TopV->next;
    free (temp);

    ViewCt -=1 ; fprintf (stderr, "POP: %d views on stack\n", ViewCt);

  } else
    prompt ("popview: can't pop, stack is empty\n");
}

void printview (v) 
     View *v;
{
  m4_print ("Np",&v->Np);
  m4_print ("Ip",&v->Ip);
  m4_print ("N2D",&v->N2D);
  fprintf (stderr, "ClipFlag: %d\n", v->ClipFlag);
}

/*
  s t r s a v e
  Allocate space for a string, copy string into space.
*/
char *strsave(s)
     char *s;
{
  char *tmp;
  
  if (s == (char *)NULL)
    return (char *)NULL;
  
  tmp = (char *) malloc((unsigned)strlen(s) + 1);
  if (tmp != (char *)NULL) {
    (void)strcpy(tmp, s);
  } else
    prompt ("strsave: could not malloc\n");

  return tmp;
}


/*
  p l a n e q

  Calculate plane equation coefficients from n points x,y,z.
  Return coefficients in c, and status=0 if points are colinear.

  Let the plane coefficients be a,b,c,d.
  Define j to be 1 if i=n, otherwise i+1. Then
      a = sum (y[i]-y[j])(z[i]+z[j])
      b = sum (z[i]-z[j])(x[i]+x[j])
      a = sum (x[i]-x[j])(y[i]+y[j])
  The value of d can then be determined by requiring one of the points
  to lie on the plane, i.e. d = [x y z 1].[a b c 0](T).
  Algorithm is from Newman & Sproull (2.ed.) p. 499.
  Routine written in RATFIV, mok 860117.
  Translated to C, mok 930215.
*/

int planeq (n, vert, s)
     int n;
     Point3 vert[];
     double s[4];
{
  register int i,j;
  double len;

  if (n < 3)			/* we can't determine plane with less */
    return -1;			/* than 3 points. */

  s[0] = s[1] = s[2] = s[3] = 0.0;

  for (i = 0; i < n; i++) {
    if (i == n-1)
      j = 0;
    else
      j = i + 1;
    s[0] = s[0] + (vert[i].y - vert[j].y) * (vert[i].z + vert[j].z);
    s[1] = s[1] + (vert[i].z - vert[j].z) * (vert[i].x + vert[j].x);
    s[2] = s[2] + (vert[i].x - vert[j].x) * (vert[i].y + vert[j].y);
  }

/* Calculate the value of d using point 1. */
  s[3] = s[0]*vert[0].x + s[1]*vert[0].y + s[2]*vert[0].z;

/* Check for colinearity. */
  if ((fabs(s[0]) < EPS) && (fabs(s[1]) < EPS) && (fabs(s[2]) < EPS))
    return 0;

  len = sqrt(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);
  s[0] /= len; s[1] /= len; s[2] /= len;
  return 1;
}

/*** end of $Source$ ***/
