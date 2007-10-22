/*
  $Id: colorlib.h 298 2006-03-17 13:21:12Z mok $
*/

#ifndef _COLORLIB_H
#define _COLORLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/* rgb color type. R, G, and B are between 0.0 and 1.0; */

typedef struct {
  float  r, g, b;
} rgbColor;


/* hsv color type. h is a float 0-360.0, s and b are floats 0.0-1.0 */

typedef struct {
  float h, s, v;
} hsvColor;

unsigned int rgb2cod (rgbColor *);

#ifdef __cplusplus
}
#endif

#endif  /* colorlib.h */

/*
Local Variables:
mode: c
mode: font-lock
End:
*/
