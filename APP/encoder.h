#ifndef __ENCODER_H__
#define __ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"

#define ENCODER_CPR             (int)0x800000       //捷昌 0x800000  恒立 0x20000
#define ENCODER_CPR_DOUBLE      (ENCODER_CPR * 2)
#define ENCODER_CPR_F           (8388608.0f)        //捷昌 8388608.0f  恒立 131072.0f
#define ENCODER_CPR_F_DOUBLE    (ENCODER_CPR_F * 2)
#define ENCODER_CPR_DIV         ((ENCODER_CPR)>>1)

#define ENCODER_TIME_DIFF       (0.00005f)

#define ENCODER_HANDLE_LEN          1


struct ENCODER_s;
struct USR_CONFIG_s;
struct TASK_s;
struct ENABLE_s;

typedef enum {
    ENCODER_GET_ANGLE = 0x02,
    ENCODER_GET_ALL_MSG = 0x1A,
    ENCODER_OUTPUT_POS_CLEAR = 0x62,
    ENCODER_INPUT_POS_CLEAR = 0xC2,
    ENCODER_CLEAR_ERROR = 0xBA,
}ENCODER_CW_t;

#pragma pack(1)

typedef struct {
    uint8_t CW;     //控制字
    uint8_t SF;     //状态标志
}ENCODER_HANDLE_t;

typedef struct {
    uint8_t ABS0;
    uint8_t ABS1;
    uint8_t ABS2;
    uint8_t bits;
    uint8_t MBS0;
    uint8_t MBS1;
    uint8_t MBS2;
    uint8_t END;
}ENCODER_POS_DATA_t;

typedef struct {
    uint8_t crc;
}ENCODER_CRC_t;

#pragma pack()

void encoder_init(struct ENCODER_s* encoder , struct ENABLE_s* enable);
void encoder_loop(struct ENCODER_s* encoder,struct USR_CONFIG_s* usr_config, struct TASK_s* task);
void encoder_tx_cp(struct ENCODER_s* encoder);
int encoder_cmd(struct ENCODER_s* encoder , uint8_t cmd);
void encoder_rx_idle(struct ENCODER_s* encoder);
void encoder_rx_half(struct ENCODER_s* encoder);
void encoder_rx_cp(struct ENCODER_s* encoder);


#ifdef __cplusplus
}
#endif



#endif
