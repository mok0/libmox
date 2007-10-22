#include <stdio.h>

typedef struct node { struct node *next; } node;
typedef struct node *NODE, NODE_REC;

static int (*Cmp)();

/* Quicksort list h, append (by induction) t at
   the end, and deposit the result in *r. */
static void qs( h, t, r)
NODE h, t, *r;
{
  NODE p, q, lo, hi;
  int nlo, nhi;

  while (h != NULL) {

    /* inductive case */
    nlo = nhi = 0; lo = hi = NULL;

    /* reverse descending prefix onto `hi'; store pivot in h; if done,
append t and rtn */
    for (q = h, p = h->next; p != NULL && (*Cmp)(h, p) >= 0; h=p, p = p->next) {
      h->next = hi; hi = h; ++nhi;
    }
    if (p == NULL) { h->next = hi; q->next = t; *r = h; return; }

    /* partition */
    for ( ; p != NULL; p = q) {
      q = p->next;

      if ((*Cmp)(p, h) < 0) {
        p->next = lo; lo = p; ++nlo;
      }
      else {
        p->next = hi; hi = p; ++nhi;
      }
    }

    /* recur */
    if (nlo < nhi) {
      qs(lo, h, r);
      r = &h->next; h = hi;     /* eliminated tail-recursive call */
    }
    else {
      qs(hi, t, &h->next);
      t = h; h = lo;            /* eliminated tail recursive call */
    }
  }
  *r = t;                       /* base case */
}

/* Sort any singly linked list where the `next' pointer is the first field
   wrt predicate `cmp'.  Uses O(lg n) stack and no other temp space. */
void lqsort( lstp, cmp )
void    *lstp;
int     (*cmp)();
{
  Cmp = cmp; qs(*(void**)lstp, NULL, lstp);
}

#ifdef TESTING

/* Quick and dirty sort utility. */

typedef struct line {
  struct line *next;
  char text[1];
} *LINE, LINE_REC;

int cmp_lines( l1, l2)
LINE l1, l2;
{
  return strcmp(l1->text, l2->text);
}

void main()
{
  char buf[1000];
  LINE lst, p;
  void *malloc();

  lst = NULL;
  while (gets(buf) != NULL) {
    p = malloc(sizeof(LINE_REC) + strlen(buf));
    strcpy(p->text, buf);
    p->next = lst; lst = p;
  }
  lqsort(&lst, cmp_lines);
  puts( "\n" );
  for (p = lst; p != NULL; p = p->next)
    puts(p->text);
}

#endif
