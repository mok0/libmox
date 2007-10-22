/*
  $Id: getword.c 226 2004-12-13 15:01:23Z mok $
*/

#include <stdio.h>
//#include <ctype.h>

#define MAXWORD 100
/* Get a word from a stream. Word separators are user definable in "is_sep".
 * Maximum word size is MAXWORD characters. If a word reaches it's maximum
 * limit, we choose not to flush the rest of the word. Returns NULL on EOF.
 */
char * 
getword(fp)
     FILE *fp;
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
