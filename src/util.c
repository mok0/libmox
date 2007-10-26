/*    
    This file is a part of moxlib, a utility library.
    Copyright (C) 1995-2007 Morten Kjeldgaard  

    This program is free software: you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <math.h>
#ifdef linux
#  include <unistd.h>		/* for myuser */
#  include <pwd.h>
#endif

/* expt(a,n)=a^n for integer n */
#define expt(a, n) pow(a, (double)(n))

/**
   Shove a string to the left, and compress spaces. 
   mk 950309.
*/
char *
strleft (char *str)
{
  register char *t, *s;

  s = str;
  while (*s) { 
    if (*s < 33) {
      t = s;
      while (*t) {
	*t = *(t+1);
	t++;
      }
    }
    else
      s++;
  }
  return str;
}

/**
   Trim spaces and junk off the end of a character string
   mk 950404.
*/
char *
strtrim (char *str)
{
  char *s;

  if (str == NULL)
    return NULL;

  s = str;
  while (*s++)			/* find end of string */
    ;

  while (*--s < 33 && s > str)	/* trim spaces off end */
    ;
  *++s = '\0';

  return str;
}

/**
   Convert a string to lower case. The string must be terminated by a 
   zero byte. mk950510.
*/
char *
strlower (char *str)
{
  char *s;

  if (str == NULL)
    return NULL;

  s = str;

  while (*s) { 
    *s = tolower(*s);
    s++;
  }
  return str;
}

/**
   Convert a string to upper case. The string must be terminated by a 
   zero byte.
*/
char *strupper (char *str)
{
  char *s;

  if (str == NULL)
    return NULL;

  s = str;

  while (*s) { 
    *s = toupper(*s);
    s++;
  }
  return str;
}

/**
   Return a character string containing the current time.  The linefeed at
   the end of the string returned by ctime is stripped off.  
   mk 950405.  
*/
char *
mytime()
{
  time_t t;
  char *s;

  t = time(NULL);
  s = ctime(&t);
  s[strlen(s)-1] = '\0';
  return s;
}

/**
   Return a character string containing the username of the user.  
   mok 950529.
*/
char *
myuser()
#ifdef linux
{
  struct passwd *p;
  p = getpwuid(getuid());
  return p->pw_name;
}
#else
{
  extern char *getlogin();
  return getlogin();
}
#endif


/**
   nicenum: find a "nice" number approximately equal to x.
   Round the number if round=1, take ceiling if round=0
   by Paul Heckbert from "Graphics Gems", Academic Press, 1990
*/

double nicenum (double x, int round)
{
  int exp;			/* exponent of x */
  double f;			/* fractional part of x */
  double nf;			/* nice, rounded fraction */

  exp = floor(log10(x));
  f = x/expt(10.0, exp);	/* between 1 and 10 */
  if (round)
    if (f<1.5) nf = 1.0;
    else if (f<3.0) nf = 2.0;
    else if (f<7.0) nf = 5.0;
    else nf = 10.0;
  else
    if (f<=1.0) nf = 1.0;
    else if (f<=2.0) nf = 2.0;
    else if (f<=5.0) nf = 5.0;
    else nf = 10.0;
  return nf*expt(10.0, exp);
}

/**
   strip off the directory bit of a filename.
   mok 950816
*/
char *
eatpath (char *fname)
{
  register int n;
  char *s;

  if (!fname)
    return NULL;

  s = fname;
  n = strlen(s);		/* find the length of it. */
  while (n--) {
    if (fname[n] == '/') {
      n++;
      return s+n; 
    }
  }
  return fname;
}

/**
   eat the file name extension bit from a filename. The string
   passed to this routine is changed.
*/
char *eatext (char *fname)
{
  register int n;

  if (!fname)
    return NULL;

  n = strlen(fname);
  while (n--) {
    if (fname[n] == '.') {
      fname[n] = '\0';
      break;
    }
  }
  return fname;
}

/**
   print out the bits of a 32 bit unsigned long. Mostly for debugging.
   mk 950330.
*/
void printbits (unsigned long num)
{
  char bbuf[33];
  int i;
  
  for (i = 32; i >= 0; ) {
    bbuf[32 - i] = (((num >> --i) & 1) == 1 ? '1' : '0');
  }
  bbuf[32 - i] = '\0';
  printf (" %s", bbuf);
}

/**
   Return 1 if two strings are equal, upto and including trailing \0, else
   return zero.
   mk 950528
*/
int streq (char *s, char *t)
{
  while (*t) {
    if (*t != *s)
      return 0;
    t++; s++;
  }

  if (*s == 0 && *t == 0)
    return 1;

  return 0;
}
  
/**
   Return 1 if two strings are equal, upto and including trailing \0, else
   return zero. Case insensitive version.
   mk 970407
*/

int strcaseeq (char *s, char *t)
{
  int tl, sl;

  while (*t) {
    tl = tolower(*t);
    sl = tolower(*s);
    if (tl != sl)
      return 0;
    t++; s++;
  }

  if (*s == 0 && *t == 0)
    return 1;

  return 0;
}

/**
  just die...
*/
void 
die (char *str)
{
  perror (str);
  exit (1);
}


double 
d_sign (double *a, double *b)
{
  double x;
  x = (*a >= 0 ? *a : -*a);
  return (*b >= 0 ? x : -x);
}

/**
   Return a random integer in the range 0:max
*/

int rand_range(int max)
{
  return (int) (10.0*rand()/(RAND_MAX+1.0));
}




/**
   See if a file exists. Try a number of different strategies to find
   the file.
*/

char *fileexists (char *name, char *env)
{
  struct stat sbuf;
  char str[512];
  char *c;

  c = NULL;
  if (env) 
    c = getenv(env);

  /* if env var is set, try that */
  if (c) {
    if (!stat(c, &sbuf))
      return strdup (c);
  }

  /* then try straight file name */
  if (name) {
    if (!stat (name, &sbuf))
      return strdup (name);

    /* prepend /usr/lib to filename */
    strcpy (str, "/usr/lib/");
    strcat (str, name);
    if (!stat (str, &sbuf))
      return strdup(str);

    /* prepend env to filename */
    if (c) {
      int i;
      strcpy (str, c);
      /* if env does not end with a /, add one */
      i = strlen(c);		
      if (c[i-1] != '/') {
	c[i] = '/';
	c[i+1] = 0;
      }
      strcat (str, name);
      if (!stat (str, &sbuf))
	return strdup(str);
    }
  }
  return NULL;
}

#ifdef TESTING
main ()
{
  char *eatpath();
  char *eatext();

  char buf[100];
  while (1) {
    printf ("Enter a string: ");
    gets (buf);
    printf ("trimmed |%s|\n", strtrim(buf));
    printf ("basename >%s<\n", eatpath(buf));
    printf ("eatext >%s<\n", eatext(buf));

  }
}

#endif

/* 
Local Variables:
mode: font-lock
End:
*/
