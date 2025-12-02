#include "battery.h"


int battery_send(struct BATTERY_s* battery  , uint8_t* data, uint8_t len)
{
    uint16_t temp_len = battery->need_send_len + len;
    if(temp_len <= ENC_SEND_BUFF_SIZE && battery->tc_flg){
        memcpy(&battery->send_buf[battery->need_send_len],data,len);
        battery->need_send_len = temp_len;
        battery->tc_flg = false;
        battery->cur_send_len = battery->need_send_len;
        HAL_UART_Transmit_DMA(battery->usart, battery->send_buf,battery->cur_send_len);
    }
    else{
        return -1;
    }
    return 0;
}

int battery_read_message(struct BATTERY_s* battery )
{
    uint8_t data[] = {0xdd,0x03,0x00,0x00,0x77};
    uint8_t check = calculate_checksum(data + 1,2);
    data[3] = check;
    if(battery_send(battery , data,sizeof(data)) != 0) {
        return -1;
    }
    return 0;
}

int battery_read_voltage(struct BATTERY_s* battery )
{
    uint8_t data[] = {0xdd,0x04,0x00,0x00,0x77};
    uint8_t check = calculate_checksum(data + 1,2);
    data[3] = check;
    if(battery_send(battery , data,sizeof(data)) != 0) {
        return -1;
    }
    return 0;
}

int battery_set_press_time(struct BATTERY_s* battery , uint16_t long_time,uint16_t short_time)
{
    uint8_t data[] = {0xdd,0x05,0x04,0x00,0x00,0x00,0x00,0x00,0x77};
    memcpy(data + 3,&short_time,2);
    memcpy(data + 5,&long_time,2);
    uint8_t check = calculate_checksum(data + 1,6);
    data[7] = check;
    if(battery_send(battery , data,sizeof(data)) != 0) {
        return -1;
    }
    return 0;
}

int battery_led_control(struct BATTERY_s* battery , uint8_t led_control,uint8_t led_4_3,uint8_t led_2_1)
{
    uint8_t data[] = {0xdd,0x05,0x03,0x00,0x00,0x00,0x00,0x77};
    memcpy(data + 3,&led_control,1);
    memcpy(data + 4,&led_4_3,1);
    memcpy(data + 5,&led_2_1,1);
    uint8_t check = calculate_checksum(data + 1,5);
    data[6] = check;
    if(battery_send(battery , data,sizeof(data)) != 0) {
        return -1;
    }
    return 0;
}

int battery_check_handle(uint8_t *data , uint16_t len)
{
    if((data[1] == BATTERY_GET_MSG || data[1] == BATTERY_GET_MONOMER_VOLTAGE || data[1] == BATTERY_SET_PRESS_TIME || data[1] == BATTERY_LED_CONTROL)
       && (data[0] == 0xdd)) {
        return RECE_DATA_PROCESS_SUCCESS;
    }
    return RECE_DATA_PROCESS_FAIL;
}

int battery_data_process(struct BATTERY_s* battery , uint8_t* data , uint16_t len ,uint16_t *rece_pack_len)
{
    BATTERY_HANDLE_t *handle;
    BATTERY_DATA_t *battery_data;
    BATTERY_CHECK_t *rece_check;
    uint16_t pack_data_length = 0;
    uint8_t check = 0;

    handle = (BATTERY_HANDLE_t *)data;
    battery_data = (BATTERY_DATA_t *)(data + sizeof(BATTERY_HANDLE_t));
    pack_data_length = sizeof(BATTERY_HANDLE_t) + handle->len + sizeof(BATTERY_CHECK_t);
    rece_check = (BATTERY_CHECK_t *)(data + sizeof(BATTERY_HANDLE_t) + handle->len);
    check = calculate_checksum(data + 1,sizeof(BATTERY_HANDLE_t) + handle->len - 1);

    if(len < pack_data_length) {
        return RECE_DATA_PROCESS_WAIT;
    }

    if(check != rece_check->check) {
        return RECE_DATA_PROCESS_FAIL;
    }

    *rece_pack_len = pack_data_length;

    switch(handle->cmdcode) {
        case BATTERY_GET_MSG: {
            battery->voltage = (battery_data->data[1] | (battery_data->data[0] << 8)) * 0.001;
            battery->current = (battery_data->data[3] | (battery_data->data[2] << 8)) * 0.01;
            battery->standard_power = (battery_data->data[5] | (battery_data->data[4] << 8)) ;
            battery->current_power = (battery_data->data[7] | (battery_data->data[6] << 8));
            battery->charging_time = (battery_data->data[9] | (battery_data->data[8] << 8));
            battery->protection_status = (battery_data->data[11] | (battery_data->data[10] << 8));
            battery->cycle_count = (battery_data->data[13] | (battery_data->data[12] << 8));
            battery->SOH = battery_data->data[14];
            battery->RSOC = battery_data->data[15];
            battery->FET_status = battery_data->data[16];
            battery->battery_num = battery_data->data[17];
            battery->NTC_temperature = ((battery_data->data[19] | (battery_data->data[18] << 8)) - 2731)/10;
            break;
        }
        case BATTERY_GET_MONOMER_VOLTAGE: {
            for(uint8_t i = 0; i < BATTERY_NUM; i++) {
                battery->monomer_battery[i] = (battery_data->data[2*i + 1] | (battery_data->data[2*i] << 8)) * 0.001;
            }
            break;
        }
        case BATTERY_SET_PRESS_TIME: {
            break;
        }
        case BATTERY_LED_CONTROL: {
            break;
        }
        default: 
            break;
    }
    return 0;
}

int battery_rece(struct BATTERY_s* battery ) //非队列形式，可能会有一帧错误
{
    int result = 0;
    int rece_start_idx = 0;
    int rece_idx = 0;
    uint16_t rece_pack_len = 0;
    battery->cur_analysis_len = battery->cur_rece_len;
    if(battery->cur_analysis_len <= 1){
        return 0;
    }
    while(battery->cur_analysis_len > 1){
        result = battery_check_handle(&battery->rcve_buf[rece_idx],battery->cur_analysis_len);
        if(result == RECE_DATA_PROCESS_FAIL) {
            battery->cur_analysis_len -= 1;
            rece_idx += 1;
            continue;
        }

        result = battery_data_process(battery,&battery->rcve_buf[rece_idx],battery->cur_analysis_len,&rece_pack_len);

        if(result == RECE_DATA_PROCESS_SUCCESS) {
            rece_idx += rece_pack_len;
            battery->cur_analysis_len -= rece_pack_len;
            continue;
        }
        else if(result == RECE_DATA_PROCESS_WAIT) {
            rece_start_idx = rece_idx;
            break;
        }
        else {
            rece_idx += 1;
            battery->cur_analysis_len -= 1;
            continue;
        }
    }
    if(battery->cur_analysis_len > 0){
        memcpy(battery->rcve_buf,battery->rcve_buf+rece_start_idx,battery->cur_analysis_len);
        battery->cur_rece_len = battery->cur_analysis_len;
    }
    else{
        battery->cur_rece_len = 0;
    }
    return 0;
}

void battery_tx_cp(struct BATTERY_s *battery)
{
    battery->tc_flg = true;
    int diff_len = battery->need_send_len - battery->cur_send_len;
    if(diff_len > 0){
        memcpy(battery->send_buf,&battery->send_buf[battery->cur_send_len],diff_len);
        battery->need_send_len = diff_len;
        battery->cur_send_len = 0;
    }
    else{
        battery->need_send_len = 0;
    }
}

void battery_rx_idle(struct BATTERY_s *battery)
{
    if(__HAL_UART_GET_FLAG(battery->usart,UART_FLAG_IDLE)){
        __HAL_UART_CLEAR_IDLEFLAG(battery->usart);
        uint16_t cnt = BAT_RECE_BUFF_SIZE - __HAL_DMA_GET_COUNTER(battery->usart->hdmarx) - battery->rx_idx;
        if(cnt > BAT_RECE_BUFF_SIZE) {
            battery->cur_rece_len = 0;
            battery->rx_idx = 0;
            return;
        }
        uint16_t all_len = cnt + battery->cur_rece_len;
        if(all_len <= BAT_RECE_BUFF_SIZE){
            memcpy(&battery->rcve_buf[battery->cur_rece_len],&battery->usart->pRxBuffPtr[battery->rx_idx],cnt);
            battery->cur_rece_len = all_len;
            battery->rx_idx = BAT_RECE_BUFF_SIZE - __HAL_DMA_GET_COUNTER(battery->usart->hdmarx);
            battery->rx_flg = true;
        }
        else{
            //to do 错误码
        }
    }
}


void battery_rx_half(struct BATTERY_s *battery)
{
    uint16_t cnt = BAT_HALF_RECE_BUFF_SIZE - battery->rx_idx;
    uint16_t all_len = cnt + battery->cur_rece_len;
    if(all_len <= BAT_RECE_BUFF_SIZE) {
        memcpy(&battery->rcve_buf[battery->cur_rece_len],&battery->usart->pRxBuffPtr[battery->rx_idx],cnt);
        battery->cur_rece_len = all_len;
        battery->rx_idx = BAT_RECE_BUFF_SIZE / 2;
        battery->rx_flg = true;
    }
    else {
        //to do
    }
}

void battery_rx_cp(struct BATTERY_s *battery)
{
    uint16_t cnt = BAT_RECE_BUFF_SIZE - battery->rx_idx;
    uint16_t all_len = cnt + battery->cur_rece_len;

    if(all_len <= BAT_RECE_BUFF_SIZE) {
        memcpy(&battery->rcve_buf[battery->cur_rece_len],&battery->usart->pRxBuffPtr[battery->rx_idx],cnt);
        battery->cur_rece_len = all_len;
        battery->rx_idx = 0;
        battery->rx_flg = true;
    }
    else {
        //to do
    }

}

void battery_loop(struct BATTERY_s *battery)
{
    battery_read_message(battery);
    battery_rece(battery);
}

void battery_init(struct BATTERY_s *battery)
{
    battery->tc_flg = true;
    battery->rx_flg = 0;
    battery->cur_rece_len = 0;
    battery->cur_analysis_len = 0;
    battery->cur_send_len = 0;
    battery->need_send_len = 0;
    battery->rx_idx = 0;
    battery->short_press_time = 0;
    battery->long_press_time = 0;
    battery->control_bit = 0;
    battery->led4_led3_status = 0;
    battery->led2_led1_status = 0;
}