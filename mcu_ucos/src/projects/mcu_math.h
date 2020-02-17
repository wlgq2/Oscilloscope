#ifndef MATH_H

#define MATH_H

#include "common_header.h"
#include "task_header.h"
#include "arm_math.h"

#define pi 3.1415926

#define    KalmanU   0

#define    KalmanQ   0.1
#define    KalmanR   100
#define    KalmanInitX (TriggerPoint-10)
#define    KalmanInitP 2

#define    KalmanLength  480

struct complex_num
{
    float real;
    float imag;
};


void fft(uint8* data,struct complex_num* data_out,uint8 fft_length,uint16 FFT_count);
void absfft(struct complex_num * fft_result,uint8 * absdata,uint32 cnt);
void FirPara(uint8 H,uint8 L, uint16 N,int32 * fir_c,uint8 len);
void KalmanFilter(uint8 * NoiseSig,uint8 InitX,uint8 InitP);

#endif

