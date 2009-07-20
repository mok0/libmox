/*
    This file is a part of moxlib, a utility library.
    Copyright (C) 1995-2009 Morten Kjeldgaard

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

#include "../src/colorlib.h"


main ()
{
  rgbColor rgb;
  hsvColor hsv;

  rgb.r = 1.; rgb.g = 0.0 ; rgb.b = 0.0;
  rgb2hsv (&rgb, &hsv); hsv2rgb (&hsv, &rgb);

  rgb.r = 0.0; rgb.g = 1.0 ; rgb.b = 0.0;
  rgb2hsv (&rgb, &hsv);
  hsv2rgb (&hsv, &rgb);

  rgb.r = 0.0; rgb.g = 0.0 ; rgb.b = 1.0;
  rgb2hsv (&rgb, &hsv);
  hsv2rgb (&hsv, &rgb);

  rgb.r = 1.; rgb.g = 1.0 ; rgb.b = 0.0;
  rgb2hsv (&rgb, &hsv);
  hsv2rgb (&hsv, &rgb);

  rgb.r = 1.; rgb.g = 0.0 ; rgb.b = 1.0;
  rgb2hsv (&rgb, &hsv);
  hsv2rgb (&hsv, &rgb);

  rgb.r = 0.; rgb.g = 1.0 ; rgb.b = 1.0;
  rgb2hsv (&rgb, &hsv);
  hsv2rgb (&hsv, &rgb);

}

