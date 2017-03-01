#ifndef _IMAGE_H_
#define _IMAGE_H_

#ifndef _UCHAR_T
#define _UCHAR_T
typedef unsigned char uchar;
#endif

uchar get_pixel( uchar* src, int x, int y, int width, int height  );
uchar clip( double pixel );

void difference( uchar* dst, uchar* s1, uchar* s2, int width, int height );

#endif
