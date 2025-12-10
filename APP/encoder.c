#include "encoder.h"

int encoder_send(struct ENCODER_s* encoder  , uint8_t* data, uint8_t len)
{
    uint16_t temp_len = encoder->need_send_len + len;
    if(temp_len <= ENC_SEND_BUFF_SIZE){
        memcpy(&encoder->send_buf[encoder->need_send_len],data,len);
        encoder->need_send_len = temp_len;
        encoder->tc_flg = false;
        encoder->cur_send_len = encoder->need_send_len;
        HAL_UART_Transmit_DMA(encoder->usart, encoder->send_buf,encoder->cur_send_len);
    }
    else{
        return -1;
    }
    return 0;
}

int encoder_cmd(struct ENCODER_s* encoder , uint8_t cmd)
{
    int ret = 0;
    if(encoder->tc_flg == true)
    ret = encoder_send(encoder, (uint8_t *)&cmd, 1);
    return ret;
}

int encoder_data_deal(struct ENCODER_s* encoder , struct USR_CONFIG_s* usr_config,struct TASK_s* task)
{    

    int32_t diff = encoder->one_pos_count - encoder->last_one_pos_count;

    while(diff > ENCODER_CPR_DIV) diff -= ENCODER_CPR;
    while(diff < -ENCODER_CPR_DIV) diff += ENCODER_CPR;

    if(ABS(diff) > usr_config->encoder_cnt_limit) {
        if(encoder->check_count == 0) {
            encoder->one_pos_count = encoder->last_one_pos_count;
            diff = 0;
        }
        else {
            diff=(encoder->delta_count_record[0]+encoder->delta_count_record[1]+encoder->delta_count_record[2]+encoder->delta_count_record[3])>>2;
        }
        if(++encoder->check_count >= 5){
            encoder->check_count = 5;
            task->statusword_new.errors.encode_error = true;
        }
    }
    else {
		encoder->delta_count_record[3]=encoder->delta_count_record[2];
		encoder->delta_count_record[2]=encoder->delta_count_record[1];
		encoder->delta_count_record[1]=encoder->delta_count_record[0];
		encoder->delta_count_record[0]=diff;
		encoder->check_count=0;  
    }


    encoder->shadow_count += diff;

    /* Run vel PLL */
    encoder->pos_cpr_count += encoder->time_diff * encoder->vel_estimate_counts;
    float diff_pos_cpr_count = (float)(encoder->one_pos_count - (int32_t)encoder->pos_cpr_count);
    while(diff_pos_cpr_count > ENCODER_CPR_DIV) diff_pos_cpr_count -= ENCODER_CPR_F;
    while(diff_pos_cpr_count < -ENCODER_CPR_DIV) diff_pos_cpr_count += ENCODER_CPR_F;
    encoder->pos_cpr_count += encoder->time_diff * encoder->pll_kp * diff_pos_cpr_count;
    while(encoder->pos_cpr_count > ENCODER_CPR) encoder->pos_cpr_count -= ENCODER_CPR_F;
    while(encoder->pos_cpr_count < 0) encoder->pos_cpr_count += ENCODER_CPR_F;
    encoder->vel_estimate_counts += encoder->time_diff * encoder->pll_ki * diff_pos_cpr_count;

    bool snap_to_zero_vel = false;
    if(ABS(encoder->vel_estimate_counts) < encoder->snap_threshold) {
        encoder->vel_estimate_counts = 0.0f;
        snap_to_zero_vel = true;
    }

    if (snap_to_zero_vel) {
        encoder->interpolation = 0.5f;
    }
    else if(diff > 0) {
        encoder->interpolation = 0.0f;
    }
    else if(diff < 0) {
        encoder->interpolation = 1.0f;
    }
    else {
        encoder->interpolation += encoder->time_diff * encoder->vel_estimate_counts;
		if (encoder->interpolation > 1.0f) encoder->interpolation = 1.0f;
		if (encoder->interpolation < 0.0f) encoder->interpolation = 0.0f;
    }

    float interpolated_enc = encoder->one_pos_count - usr_config->encoder_offset + encoder->interpolation;
    while(interpolated_enc > ENCODER_CPR_F) interpolated_enc -= ENCODER_CPR_F;
    while(interpolated_enc < 0) interpolated_enc += ENCODER_CPR_F;

    float shadow_count_f = encoder->shadow_count;

    encoder->pos = shadow_count_f / ENCODER_CPR_F;
    encoder->vel = encoder->vel_estimate_counts / ENCODER_CPR_F;
    encoder->phase = (interpolated_enc * M_2PI * usr_config->motor_pole_pairs) / ENCODER_CPR_F;
    
    encoder->phase_count = encoder->pos * M_2PI;
    encoder->phase_vel = encoder->vel * M_2PI * usr_config->motor_pole_pairs;
    
    return 0;
}

int encoder_check_handle(uint8_t *data , uint16_t len)
{
    if(data[0] == ENCODER_GET_ANGLE || data[0] == ENCODER_OUTPUT_POS_CLEAR || data[0] == ENCODER_INPUT_POS_CLEAR || data[0] == ENCODER_CLEAR_ERROR
     || data[0] == ENCODER_GET_ALL_MSG) {
        if(len >= (sizeof(ENCODER_HANDLE_t) + sizeof(ENCODER_POS_DATA_t) + sizeof(ENCODER_CRC_t))) {
            return RECE_DATA_PROCESS_SUCCESS;
        }
        else{
            return RECE_DATA_PROCESS_WAIT;
        }
    }
    
    return RECE_DATA_PROCESS_FAIL;
}

int encoder_data_process(struct ENCODER_s* encoder , uint8_t* data , uint16_t len ,uint16_t *rece_pack_len , struct USR_CONFIG_s* usr_config , struct TASK_s* task)
{
    ENCODER_HANDLE_t *handle;
    ENCODER_POS_DATA_t *encoder_data;
    ENCODER_CRC_t *rece_crc;
    uint16_t pack_data_length = 0;
    uint8_t crc = 0;

    handle = (ENCODER_HANDLE_t *)data;
    encoder_data = (ENCODER_POS_DATA_t *)(data + sizeof(ENCODER_HANDLE_t));
    pack_data_length = sizeof(ENCODER_HANDLE_t) + sizeof(ENCODER_POS_DATA_t) + sizeof(ENCODER_CRC_t);
    rece_crc = (ENCODER_CRC_t *)(data + sizeof(ENCODER_HANDLE_t) + sizeof(ENCODER_POS_DATA_t));
    crc = APP_Math_CRC8_StaticTable(data,sizeof(ENCODER_HANDLE_t) + sizeof(ENCODER_POS_DATA_t));

    if(len < pack_data_length) 
    {
        return RECE_DATA_PROCESS_WAIT;
    }

    if(crc != rece_crc->crc) {
        return RECE_DATA_PROCESS_FAIL;
    }
    *rece_pack_len = pack_data_length;

    switch(handle->CW)
    {
        case ENCODER_GET_ALL_MSG: {
            encoder->one_pos_count = encoder_data->ABS0 | encoder_data->ABS1 << 8 | encoder_data->ABS2 << 16;
            encoder->status_flg = handle->SF;
            encoder->more_pos_count = encoder_data->MBS0 | encoder_data->MBS1 << 8 | encoder_data->MBS2 << 16;
            encoder_data_deal(encoder,usr_config,task);
            encoder->last_one_pos_count = encoder->one_pos_count;
            break;
        }
        case ENCODER_GET_ANGLE: {
            break;
        }
        case ENCODER_OUTPUT_POS_CLEAR: {
            break;
        }
        case ENCODER_INPUT_POS_CLEAR: {
            break;
        }
        case ENCODER_CLEAR_ERROR: {
            break;
        }
    }
    return RECE_DATA_PROCESS_SUCCESS;
}

int encoder_rece(struct ENCODER_s* encoder , struct USR_CONFIG_s* usr_config , struct TASK_s* task) //非队列形式，可能会有一帧错误
{
    int result = 0;
    int rece_start_idx = 0;
    int rece_idx = 0;
    uint16_t rece_pack_len = 0;
    encoder->cur_analysis_len = encoder->cur_rece_len;
    if(encoder->cur_analysis_len <= 1){
        return 0;
    }
    while(encoder->cur_analysis_len > 1){
        result = encoder_check_handle(&encoder->rcve_buf[rece_idx],encoder->cur_analysis_len);
        if(result == RECE_DATA_PROCESS_WAIT) {
            rece_start_idx = rece_idx;
            break;
        }
        else if(result == RECE_DATA_PROCESS_FAIL) {
            encoder->cur_analysis_len -= 1;
            rece_idx += 1;
            continue;
        }
        result = encoder_data_process(encoder,&encoder->rcve_buf[rece_idx],encoder->cur_analysis_len,&rece_pack_len , usr_config,task);

        if(result == RECE_DATA_PROCESS_SUCCESS) {
            rece_idx += rece_pack_len;
            encoder->cur_analysis_len -= rece_pack_len;
            continue;
        }
        else if(result == RECE_DATA_PROCESS_WAIT) {
            rece_start_idx = rece_idx;
            break;
        }
        else {
            rece_idx += 1;
            encoder->cur_analysis_len -= 1;
            continue;
        }
    }
    if(encoder->cur_analysis_len > 0){
        memcpy(encoder->rcve_buf,encoder->rcve_buf+rece_start_idx,encoder->cur_analysis_len);
        encoder->cur_rece_len = encoder->cur_analysis_len;
    }
    else{
        encoder->cur_rece_len = 0;
    }
    return 0;
}

void encoder_tx_cp(struct ENCODER_s* encoder)
{
    encoder->tc_flg = true;
    int diff_len = encoder->need_send_len - encoder->cur_send_len;
    if(diff_len > 0){
        memcpy(encoder->send_buf,&encoder->send_buf[encoder->cur_send_len],diff_len);
        encoder->need_send_len = diff_len;
        encoder->cur_send_len = 0;
    }
    else{
        encoder->need_send_len = 0;
    }
}


void encoder_rx_idle(struct ENCODER_s* encoder)
{
    if(__HAL_UART_GET_FLAG(encoder->usart,UART_FLAG_IDLE)){
        __HAL_UART_CLEAR_IDLEFLAG(encoder->usart);
        uint16_t cnt = ENC_RECE_BUFF_SIZE - __HAL_DMA_GET_COUNTER(encoder->usart->hdmarx) - encoder->rx_idx;
        if(cnt > ENC_RECE_BUFF_SIZE) {
            encoder->cur_rece_len = 0;
            encoder->rx_idx = 0;
            return;
        }
        uint16_t all_len = cnt + encoder->cur_rece_len;
        if(all_len <= ENC_RECE_BUFF_SIZE){
            memcpy(&encoder->rcve_buf[encoder->cur_rece_len],&encoder->usart->pRxBuffPtr[encoder->rx_idx],cnt);
            encoder->cur_rece_len = all_len;
            encoder->rx_idx = ENC_RECE_BUFF_SIZE - __HAL_DMA_GET_COUNTER(encoder->usart->hdmarx);
            encoder->rx_flg = true;
        }
        else{
            //to do 错误码
        }
    }
}

void encoder_rx_half(struct ENCODER_s* encoder)
{
    uint16_t cnt = ENC_HALF_RECE_BUFF_SIZE - encoder->rx_idx;
    uint16_t all_len = cnt + encoder->cur_rece_len;
    if(all_len <= ENC_RECE_BUFF_SIZE){
        memcpy(&encoder->rcve_buf[encoder->cur_rece_len],&encoder->usart->pRxBuffPtr[encoder->rx_idx],cnt);
        encoder->cur_rece_len = all_len;
        encoder->rx_idx = ENC_RECE_BUFF_SIZE / 2;
        encoder->rx_flg = true;
    }
    else{
        //to do 错误码
    }
    
}

void encoder_rx_cp(struct ENCODER_s* encoder)
{
    uint16_t cnt = ENC_RECE_BUFF_SIZE - encoder->rx_idx;
    uint16_t all_len = cnt + encoder->cur_rece_len;
    if(all_len <= ENC_RECE_BUFF_SIZE){
        memcpy(&encoder->rcve_buf[encoder->cur_rece_len],&encoder->usart->pRxBuffPtr[encoder->rx_idx],cnt);
        encoder->cur_rece_len = all_len;
        encoder->rx_idx = 0;
        encoder->rx_flg = true;
    }
    else{
        //to do 错误码
    }
}

void encoder_init(struct ENCODER_s* encoder , struct ENABLE_s* enable)
{
    encoder->tc_flg = true;
    encoder->rx_flg = false;
    encoder->rx_idx = 0;
    encoder->shadow_count = 0;
    encoder->pos_cpr_count = 0;
    encoder->vel_estimate_counts = 0;
    encoder->pos = 0;
    encoder->vel = 0;
    encoder->phase = 0;
    encoder->phase_vel = 0;
    encoder->interpolation = 0;
    encoder->one_pos_count = 0;
    encoder->last_one_pos_count = 0;
    encoder->time_diff = CURRENT_MEASURE_PERIOD;
    encoder->ele_phase = 0;
    HAL_GPIO_WritePin(enable->encoder_enable_gpio, enable->encoder_enable_pin, GPIO_PIN_SET);

    int encoder_pll_bw = 100;
    float bandwidth = MIN(encoder_pll_bw, 0.25f * 1000);
    encoder->pll_kp = 2.0f * bandwidth;                  
    encoder->pll_ki = 0.25f * SQ(encoder->pll_kp);         
    encoder->snap_threshold = 0.5f * encoder->time_diff * encoder->pll_ki;
    APP_Math_CRC8_GenerateTable();
}

void encoder_loop(struct ENCODER_s* encoder,struct USR_CONFIG_s* usr_config,struct TASK_s* task)
{
    encoder_cmd(encoder,ENCODER_GET_ALL_MSG);
    encoder_rece(encoder , usr_config , task);
    if(encoder->rx_flg == false) {
        encoder->rx_delay_count++;
        if(encoder->rx_delay_count > 1000) {
            task->statusword_new.errors.encode_error = true;
        }
    }
    else {
        encoder->rx_delay_count = 0;
        encoder->rx_flg = false;
    }
}

