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
#include <ctype.h>
#define MAXWORD 1024

/**
   @file getword.c
*/


/**
    Get a word from a stream.  Maximum word size is MAXWORD
    characters. If a word reaches it's maximum limit, we choose not to
    flush the rest of the word. Returns NULL on EOF.
*/
char *getword(FILE *fp)
{
  static char word[MAXWORD + 1];
  char *p = word;
  int c;

  while ((c = fgetc(fp)) != EOF && isspace(c))
    ;				/* Skip over word separators */
  if (c == EOF)
    return(NULL);

  *p++ = c;
  while ((c = fgetc(fp)) != EOF && !isspace(c) && p != &(word[MAXWORD])) {
    *p++ = c;			/* Quit when a word separator is encountered
				 * or we reach maximum word length
				 */
  }
  *p = '\0';			/* Mustn't forget that word terminator */
  return ((c == EOF) ? NULL : word);
}


#ifdef TESTING

main ()
{
  char *word;
  FILE *f;

  f = fdopen (0, "r");

  while (1) {

    word = getword(f);
    if (!word)
      return 0;
    puts (word);
  }
}
#endif

/*
  Local variables:
  mode: font-lock
  End:
*/
