#ifndef __MAIN_COMMUNICATION_H__
#define __MAIN_COMMUNICATION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"

struct MAIN_COMM_s;
struct USR_CONFIG_s;
struct TASK_s;
struct ENCODER_s;
struct CONTROLLER_s;
struct ENABLE_s;

#pragma pack(1)

typedef enum {
    MAIN_COMM_POS_SPEED_TIME = 0x4000,
    MAIN_COMM_POS = 0x4001,
    MAIN_COMM_SPEED = 0x4002,
    MAIN_COMM_CLEAR_ERROR = 0x4003,
}MAIN_COMM_CW_t;

typedef struct {
    uint16_t cw;     //控制字
    uint8_t data_len;     //数据长度
}MAIN_COMM_HANDLE_t;

typedef struct {
    float pos;
    float vel;
    float time;
}MAIN_COMM_DATA_t;

typedef struct {
    uint16_t crc_16;
}MAIN_COMM_CRC_t;

typedef struct {
    float pos;
    float vel;
    uint32_t status_code;
    uint32_t error_code;
}MAIN_COMM_RESPON_t;

#pragma pack()

void main_communication_tx_cp(struct MAIN_COMM_s *comm);
void main_communication_rx_idle(struct MAIN_COMM_s *comm);
void main_communication_rx_half(struct MAIN_COMM_s *comm);
void main_communication_rx_cp(struct MAIN_COMM_s *comm);
void main_communication_loop(struct MAIN_COMM_s *comm , struct USR_CONFIG_s *config, struct TASK_s *task,struct ENCODER_s* encoder , struct CONTROLLER_s* controller,struct ENABLE_s *enable);
void main_communication_init(struct MAIN_COMM_s *comm);



#ifdef __cplusplus
}
#endif


#endif