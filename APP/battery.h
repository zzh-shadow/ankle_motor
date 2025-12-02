#ifndef __BATTERY_H__
#define __BATTERY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"

struct BATTERY_s;

#pragma pack(1)

typedef enum {
    BATTERY_GET_MSG = 0X03,
    BATTERY_GET_MONOMER_VOLTAGE = 0X04,
    BATTERY_SET_PRESS_TIME = 0X05,
    BATTERY_LED_CONTROL = 0X06,
}BATTERY_CW_t;

typedef struct {
    uint8_t handle;
    uint8_t cmdcode;
    uint8_t len;
}BATTERY_HANDLE_t;

typedef struct {
    uint8_t data[30];
}BATTERY_DATA_t;

typedef struct {
    uint8_t check;
    uint8_t stop;
}BATTERY_CHECK_t;

#pragma pack()


void battery_loop(struct BATTERY_s *battery);
void battery_init(struct BATTERY_s *battery);
void battery_rx_cp(struct BATTERY_s *battery);
void battery_rx_half(struct BATTERY_s *battery);
void battery_rx_idle(struct BATTERY_s *battery);
void battery_tx_cp(struct BATTERY_s *battery);
int battery_read_voltage(struct BATTERY_s* battery );
int battery_set_press_time(struct BATTERY_s* battery , uint16_t long_time,uint16_t short_time);
int battery_led_control(struct BATTERY_s* battery , uint8_t led_control,uint8_t led_4_3,uint8_t led_2_1);



#ifdef __cplusplus
}
#endif



#endif