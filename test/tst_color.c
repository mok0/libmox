


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

