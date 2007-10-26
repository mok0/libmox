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

#include "colorlib.h"

/**
  Translate a colour given in hue, saturation and value (intensity) as
  given by the PS300 into an (R,G,B) triplet.  See Foley & Van Dam
  p. 615.  First version, in Fortran, 11-May-1990 Morten Kjeldgaard
  Converted to C 991104.  */
void hsv2rgb (hsvColor *hsv, rgbColor *rgb)
{
  float f, p, q, t;
  float r, g, b, h, s, v;
  int i;

  r = g = b = 0.0;
  h = hsv->h; s = hsv->s; v = hsv->v;

  if (s == 0.0) {
   r = v; g = v; b = v;
  }  else {

    h = h - 120.0;

    if (h < 0.0) h = h + 360.0;

    if (h >= 360.0) h = h - 360.0;

    h = h/60.0;
    i = h;
    f = h - (float)i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s*f));
    t = v * (1.0 - (s*(1.0 - f)));

    switch (i) {
    case 0:
      r = v; g = t; b = p;
      break;
    case 1:
      r = q; g = v; b = p;
      break;
    case 2:
      r = p; g = v; b = t;
      break;
    case 3:
      r = p; g = q; b = v;
      break;
    case 4:
      r = t; g = p; b = v;
      break;
    case 5:
      r = v; g = p; b = q;
    }
  }

  rgb->r = r;
  rgb->g = g;
  rgb->b = b;

#ifdef TESTING
  printf ("hsv %f %f %f -> rgb %f %f %f\n", 
	  hsv->h, hsv->s, hsv->v, rgb->r, rgb->g, rgb->b);

#endif
  return;
}

/**
 Translate a colour given in the (R,G,B) triplet into hue, saturation,
  and value (intensity) as required by the PS300.  See Foley & Van Dam
  p. 615. 10-May-1990 Morten Kjeldgaard Written, in Dallas.  Converted
  to C 991104. 
*/

void 
rgb2hsv (rgbColor *rgb, hsvColor *hsv)
{
  float r, g, b, h, s, v;
  float rgbmax, rgbmin, q, rc, gc, bc;

  h = 0.0;
  r = rgb->r; g = rgb->g; b = rgb->b;
 
  rgbmax = (r > g ? r : g); rgbmax = (rgbmax > b ? rgbmax : b);
  rgbmin = (r < g ? r : g); rgbmin = (rgbmin < b ? rgbmin : b);
  q = rgbmax - rgbmin;
  v = rgbmax;
  if (rgbmax !=  0.0) 
    s = q/rgbmax;
  else
    s = 0.0;

  if (s == 0.0)
    h = 0.0;
  else {
    /* rc measures the distance of color from red */
    rc = (rgbmax - r)/q;
    gc = (rgbmax - g)/q;
    bc = (rgbmax - b)/q;

    if (r == rgbmax) 
      /* resulting color between yellow and magenta */
      h = bc - gc;
    else if (g == rgbmax)
      /* resulting color between cyan and yellow */
      h = 2.0 + rc - bc;
    else if (b == rgbmax)
      /* resulting color between magenta and cyan */
      h = 4.0 + gc - rc;

    /* convert to degrees */
    h = h * 60.0 + 120.0;
    if (h < 0.0) h = h + 360.0;
    if (h > 360.0) h = h - 360.0;
  }
  hsv->h = h;
  hsv->s = s;
  hsv->v = v;

#ifdef TESTING
  printf ("rgb %f %f %f -> hsv %f %f %f\n", 
	  rgb->r, rgb->g, rgb->b, hsv->h, hsv->s, hsv->v);
#endif

  return;
}

/**
  Convert an rgb triplet to an O packed color cod
*/

unsigned int rgb2cod (rgbColor *rgb)
{
  int ir = (rgb->r * 255.0);
  int ig = (rgb->g * 255.0);
  int ib = (rgb->b * 255.0);
  return 256 * (256 * ir + ig) + ib;
}


/*
Local Variables:
mode: c
mode: font-lock
End:
*/
