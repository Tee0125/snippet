
#ifndef _WAVELET_H_
#define _WAVELET_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TWOTWO	0
#define SWE		1
#define CRF		2

void wavelet_encoder_jty( unsigned char* ifile, unsigned char* ofile, char filter, int label );
void wavelet_decoder_jty( unsigned char* ifile, unsigned char* ofile, char filter, int label );

#ifdef __cplusplus
}
#endif

#endif
