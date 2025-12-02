

#ifndef __UTIL_H__
#define __UTIL_H__

#include "main.h"
#include <math.h>
#include "app_pubinclude.h"

// #define M_PI                    (3.14159265358f)
#define M_2PI                    (6.28318530716f)
#define M_4PI                    (12.5663706144f)   
#define ONE_BY_SQRT3            (0.57735026919f)
#define TWO_BY_SQRT3            (2.0f * 0.57735026919f)
#define SQRT3_BY_2                (0.86602540378f)

#define SQ(x)        ((x)*(x))
#define ABS(x)         ( (x)>0?(x):-(x) ) 
#define MAX(x, y)     (((x) > (y)) ? (x) : (y))
#define MIN(x, y)     (((x) < (y)) ? (x) : (y))
#define CLAMP(x, lower, upper)      (MIN(upper, MAX(x, lower)))
#define FLOAT_EQU(floatA, floatB)   ((ABS((floatA)-(floatB))) < 0.000001f)

typedef union Resolve
{
    float    float_data;
	uint32_t uint_data;
	int16_t  short_data;
    char     char_table[4];
}Resolve_Typedef;

/**
 * A simple low pass filter.
 *
 * @param value
 * The filtered value.
 *
 * @param sample
 * Next sample.
 *
 * @param filter_constant
 * Filter constant. Range 0.0 to 1.0, where 1.0 gives the unfiltered value.
 */
#define UTILS_LP_FAST(value, sample, filter_constant)    (value -= (filter_constant) * ((value) - (sample)))

/**
 * A fast approximation of a moving average filter with N samples. See
 * https://en.wikipedia.org/wiki/Moving_average#Exponential_moving_average
 * https://en.wikipedia.org/wiki/Exponential_smoothing
 *
 * It is not entirely the same as it behaves like an IIR filter rather than a FIR filter, but takes
 * much less memory and is much faster to run.
 */
#define UTILS_LP_MOVING_AVG_APPROX(value, sample, N)    UTILS_LP_FAST(value, sample, 2.0f / ((N) + 1.0f))

void clarke_transform(float Ia, float Ib, float Ic, float *Ialpha, float *Ibeta);
void park_transform(float Ialpha, float Ibeta, float Theta, float *Id, float *Iq);
void inverse_park(float mod_d, float mod_q, float Theta, float *mod_alpha, float *mod_beta);
int svm(float alpha, float beta, float* tA, float* tB, float* tC);

float sin_f32(float x);
float cos_f32(float x);
float atan_f32(float y, float x);

uint8_t crc8(const uint8_t *data, const uint32_t size);
uint32_t crc32(const uint8_t *data, uint32_t size);
uint8_t APP_Math_CRC8_StaticTable(uint8_t *message, uint8_t len);
void APP_Math_CRC8_GenerateTable(void);

int uint32_to_data(uint32_t val, uint8_t *data);
int int32_to_data(int32_t val, uint8_t *data);
int uint16_to_data(uint16_t val, uint8_t *data);
int int16_to_data(int16_t val, uint8_t *data);
int float_to_data(float val, uint8_t *data);

uint32_t data_to_uint32(uint8_t *data);
int32_t data_to_int32(uint8_t *data);
uint16_t data_to_uint16(uint8_t *data);
int16_t data_to_int16(uint8_t *data);
float data_to_float(uint8_t *data);

int float_to_uint(float x, float x_min, float x_max, int bits);
float uint_to_float(int x_int, float x_min, float x_max, int bits);
uint8_t APP_Math_CRC8_ChkValue(uint8_t *message, uint8_t len);
uint8_t calculate_checksum(const uint8_t *data, uint8_t data_len);
uint16_t ModbusCRC16_Table(uint8_t *data, uint16_t length);
float float_swap_endian(float value);


#endif
