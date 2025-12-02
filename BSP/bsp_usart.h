#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#ifdef __cplusplus
extern "C" {
#endif


#define MAIN_RECE_BUFF_SIZE   256
#define MAIN_HALF_RECE_BUFF_SIZE   MAIN_RECE_BUFF_SIZE / 2
#define MAIN_SEND_BUFF_SIZE   256

#define ENC_RECE_BUFF_SIZE   512
#define ENC_HALF_RECE_BUFF_SIZE   ENC_RECE_BUFF_SIZE / 2
#define ENC_SEND_BUFF_SIZE   512

#define BAT_RECE_BUFF_SIZE   256
#define BAT_HALF_RECE_BUFF_SIZE   BAT_RECE_BUFF_SIZE / 2
#define BAT_SEND_BUFF_SIZE   256

void bsp_usart_init();
void bsp_uart_idle_callback(void *uart);


#ifdef __cplusplus
}
#endif


#endif  //__bsp_usart_H__