#ifndef _GAUSSIAN_H_
#define _GAUSSIAN_H_

#ifndef _UCHAR_T
#define _UCHAR_T
typedef unsigned char uchar;
#endif

#define MASK_LEN 255

void gaussian_blur( uchar* dst, uchar* src, int sigma, int w, int h );

#endif
