//Spectrum.h

#ifndef _SPECTRUM_H
#define _SPECTRUM_H

#include "types.h"

BOOL Spectrum_peakProc_ex(float *position, float* sigma);
BOOL Spectrum_QuickPeakCalculation(long left, long right, float *position, float *sigma);


#endif	//#ifndef _SPECTRUM_H