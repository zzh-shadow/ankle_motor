#include "bsp_usart.h"
#include "app_pubinclude.h"

uint8_t usart1_dma_rx_buf[MAIN_RECE_BUFF_SIZE];

uint8_t usart2_dma_rx_buf[ENC_RECE_BUFF_SIZE];

uint8_t usart3_dma_rx_buf[BAT_RECE_BUFF_SIZE];

extern MOTOR_t motor[NUM_MOTORS];

void bsp_usart_init()
{
    HAL_UART_Receive_DMA (&huart1, usart1_dma_rx_buf,MAIN_RECE_BUFF_SIZE);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);
    __HAL_DMA_ENABLE_IT(huart1.hdmarx, DMA_IT_HT);
    __HAL_DMA_ENABLE_IT(huart1.hdmarx, DMA_IT_TC);

    HAL_UART_Receive_DMA (&huart2, usart2_dma_rx_buf,ENC_RECE_BUFF_SIZE);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&huart2);
    __HAL_DMA_ENABLE_IT(huart2.hdmarx, DMA_IT_HT);
    __HAL_DMA_ENABLE_IT(huart2.hdmarx, DMA_IT_TC);
    // HAL_UART_Receive_IT(&huart1, usart2_dma_rx_buf, ENC_RECE_BUFF_SIZE);

    HAL_UART_Receive_DMA (&huart3, usart3_dma_rx_buf,BAT_RECE_BUFF_SIZE);
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    __HAL_UART_CLEAR_IDLEFLAG(&huart3);
    __HAL_DMA_ENABLE_IT(huart3.hdmarx, DMA_IT_HT);
    __HAL_DMA_ENABLE_IT(huart3.hdmarx, DMA_IT_TC);
}

void bsp_uart_idle_callback(void *uart)
{
    UART_HandleTypeDef *huart = (UART_HandleTypeDef *)uart;
    if(huart->Instance == USART1) {
        motor[0].main_comm.rx_idle(&motor[0].main_comm);
    }
    else if(huart->Instance == USART2) {
        motor[0].encoder.rx_idle(&motor[0].encoder);
    }
    else if(huart->Instance == USART3) {
        motor[0].battery.rx_idle(&motor[0].battery);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1) {
        motor[0].main_comm.tx_cp(&motor[0].main_comm);
    }
    else if(huart->Instance == USART2) {
        motor[0].encoder.tx_cp(&motor[0].encoder);
    }
    else if(huart->Instance == USART3) {
        motor[0].battery.tx_cp(&motor[0].battery);
    }
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1) {
        motor[0].main_comm.rx_half(&motor[0].main_comm);
    }
    else if(huart->Instance == USART2) {
        motor[0].encoder.rx_half(&motor[0].encoder);
    }
    else if(huart->Instance == USART3) {
        motor[0].battery.rx_half(&motor[0].battery);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1) {
        motor[0].main_comm.rx_cp(&motor[0].main_comm);
    }
    else if(huart->Instance == USART2) {
        motor[0].encoder.rx_cp(&motor[0].encoder);
    }
    else if(huart->Instance == USART3) {
        motor[0].battery.rx_cp(&motor[0].battery);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1) {
        HAL_UART_Receive_DMA (&huart1, usart1_dma_rx_buf,MAIN_RECE_BUFF_SIZE);
        __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
        __HAL_DMA_ENABLE_IT(huart1.hdmarx, DMA_IT_HT);
        __HAL_DMA_ENABLE_IT(huart1.hdmarx, DMA_IT_TC);
    }
    else if(huart->Instance == USART2) {
        HAL_UART_Receive_DMA (&huart2, usart2_dma_rx_buf,ENC_RECE_BUFF_SIZE);
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
        __HAL_DMA_ENABLE_IT(huart2.hdmarx, DMA_IT_HT);
        __HAL_DMA_ENABLE_IT(huart2.hdmarx, DMA_IT_TC);
    }
    else if(huart->Instance == USART3) {
        HAL_UART_Receive_DMA (&huart3, usart3_dma_rx_buf,ENC_RECE_BUFF_SIZE);
        __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);
        __HAL_DMA_ENABLE_IT(huart3.hdmarx, DMA_IT_HT);
        __HAL_DMA_ENABLE_IT(huart3.hdmarx, DMA_IT_TC);
    }
}