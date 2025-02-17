/*
    This file is a part of libmox, a utility library.
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

#ifndef _colorlib_h
#define _colorlib_h

#ifdef __cplusplus
extern "C" {
#endif

  /**
     rgb color type. R, G, and B are between 0.0 and 1.0;
  */
  typedef struct {
    float  r;  /*!< Red component */
    float  g;  /*!< Green component */
    float  b;  /*!< Blue component */
  } rgbColor;


  /**
  hsv color type. h is a float 0-360.0, s and b are floats 0.0-1.0
  */
  typedef struct {
    float h;  /*!< Hue (0-360) */
    float s;  /*!< Saturation (0-1) */
    float v;  /*!< Value (0-1) */
  } hsvColor;

  unsigned int rgb2cod (rgbColor *);
  void hsv2rgb (hsvColor *hsv, rgbColor *rgb);
  void rgb2hsv (rgbColor *rgb, hsvColor *hsv);


#ifdef __cplusplus
}
#endif

#endif  /* _colorlib_h */
