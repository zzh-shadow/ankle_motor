#include "main_communication.h"

int main_comm_send(struct MAIN_COMM_s *comm, uint8_t *buf, uint16_t len)
{
    uint16_t temp_len = comm->need_send_len + len;
    if(temp_len <= MAIN_SEND_BUFF_SIZE) {
        memcpy(&comm->send_buf[comm->need_send_len],buf,len);
        comm->need_send_len = temp_len;
        comm->tc_flg = false;
        comm->cur_send_len = comm->need_send_len;
        HAL_UART_Transmit_DMA(comm->usart, comm->send_buf,comm->cur_send_len);
    }
    else {
        return -1;
    }
    return 0;
}

int main_comm_response(struct MAIN_COMM_s *comm, uint16_t cmd, struct ENCODER_s* encoder ,struct TASK_s* task,struct USR_CONFIG_s* usrconfig)
{
    MAIN_COMM_HANDLE_t handle = {0};
    uint8_t data_buff[50];
    MAIN_COMM_CRC_t crc = {0};
    handle.cw = SWAP_16(cmd);
    MAIN_COMM_RESPON_t pos_vel = {0};
    if(usrconfig->invert_motor_dir) {
        pos_vel.pos = float_swap_endian(-encoder->pos);
        pos_vel.vel = float_swap_endian(-encoder->vel);
    }
    else {
        pos_vel.pos = float_swap_endian(encoder->pos);
        pos_vel.vel = float_swap_endian(encoder->vel);
    }
    pos_vel.status_code = SWAP_32(task->statusword_new.status.status_code);
    pos_vel.error_code = SWAP_32(task->statusword_new.errors.errors_code);
    handle.data_len = sizeof(MAIN_COMM_RESPON_t);
    memcpy(data_buff,&handle,sizeof(MAIN_COMM_HANDLE_t));
    memcpy(data_buff + sizeof(MAIN_COMM_HANDLE_t),&pos_vel,handle.data_len);
    crc.crc_16 = ModbusCRC16_Table(data_buff,sizeof(MAIN_COMM_HANDLE_t) + handle.data_len);
    memcpy(data_buff + sizeof(MAIN_COMM_HANDLE_t) + handle.data_len,&crc.crc_16,sizeof(MAIN_COMM_CRC_t));
    main_comm_send(comm,data_buff,handle.data_len + sizeof(MAIN_COMM_CRC_t) + sizeof(MAIN_COMM_HANDLE_t));
    return 0;
}

int main_comm_check_handle(uint8_t *data , uint16_t len) 
{
    uint16_t temp_data = data[0] << 8 | data[1];
    if(temp_data == MAIN_COMM_POS_SPEED_TIME || temp_data == MAIN_COMM_POS || temp_data == MAIN_COMM_SPEED || temp_data == MAIN_COMM_CLEAR_ERROR)  {
        if(len >= (sizeof(MAIN_COMM_HANDLE_t) + data[2] + sizeof(MAIN_COMM_CRC_t))) {
            return RECE_DATA_PROCESS_SUCCESS;
        }
        else{
            return RECE_DATA_PROCESS_WAIT;
        }
    }
    
    return RECE_DATA_PROCESS_FAIL;
}

int main_comm_data_process(struct MAIN_COMM_s* main_comm , uint8_t* data , uint16_t len ,uint16_t *rece_pack_len , struct USR_CONFIG_s* usr_config , struct TASK_s* task , struct ENCODER_s* encoder)
{
    MAIN_COMM_HANDLE_t *handle;
    MAIN_COMM_DATA_t *main_comm_data;
    MAIN_COMM_CRC_t *rece_crc;
    uint16_t pack_data_length = 0;
    uint16_t crc = 0;

    handle = (MAIN_COMM_HANDLE_t *)data;
    pack_data_length = handle->data_len + sizeof(MAIN_COMM_HANDLE_t) + sizeof(MAIN_COMM_CRC_t);
    if(len < pack_data_length) {
        return RECE_DATA_PROCESS_WAIT;
    }
    rece_crc = (MAIN_COMM_CRC_t *)(data + sizeof(MAIN_COMM_HANDLE_t) + handle->data_len);
    crc = ModbusCRC16_Table(data,sizeof(MAIN_COMM_HANDLE_t) + handle->data_len);

    if(crc != rece_crc->crc_16) {
        return RECE_DATA_PROCESS_FAIL;
    }

    main_comm_data = (MAIN_COMM_DATA_t *)(data + sizeof(MAIN_COMM_HANDLE_t));
    handle->cw = SWAP_16(handle->cw);
    switch(handle->cw) {
        case MAIN_COMM_POS_SPEED_TIME: {
            if(handle->data_len != 12) {
                return RECE_DATA_PROCESS_FAIL;
            }
            main_comm_response(main_comm,handle->cw,encoder,task,usr_config);
            main_comm->target_cw = CONTROL_MODE_POSITION_PROFILE;
            main_comm->target_pos = float_swap_endian(main_comm_data->pos);
            main_comm->target_vel = float_swap_endian(main_comm_data->vel);
            main_comm->control_time = float_swap_endian(main_comm_data->time);
            
            break;
        }
        case MAIN_COMM_POS: {
            if(handle->data_len != 4) {
                return RECE_DATA_PROCESS_FAIL;
            }
            main_comm_response(main_comm,handle->cw,encoder,task,usr_config);
            main_comm->target_cw = CONTROL_MODE_POSITION_FILTER;
            main_comm->target_pos = float_swap_endian(main_comm_data->pos);
            break;
        }
        case MAIN_COMM_SPEED: {
            if(handle->data_len != 4) {
                return RECE_DATA_PROCESS_FAIL;
            }
            main_comm_response(main_comm,handle->cw,encoder,task,usr_config);
            main_comm->target_cw = CONTROL_MODE_VELOCITY_RAMP;
            main_comm->target_vel = float_swap_endian(main_comm_data->pos);
            break;
        }
        case MAIN_COMM_CLEAR_ERROR: {
            main_comm_response(main_comm,handle->cw,encoder,task,usr_config);
            main_comm->error_clear_flg = true;
            main_comm->target_vel = encoder->vel;
            main_comm->target_pos = encoder->pos;
            break;
        }
        default:
        break;   
    }
    *rece_pack_len = pack_data_length;
    return RECE_DATA_PROCESS_SUCCESS;
}

int main_comm_rece(struct MAIN_COMM_s* main_comm,struct USR_CONFIG_s* usr_config , struct TASK_s* task,struct ENCODER_s* encoder)
{
    int result = 0;
    int rece_start_idx = 0;
    int rece_idx = 0;
    uint16_t rece_pack_len = 0;
    main_comm->cur_analysis_len = main_comm->cur_rece_len;
    if(main_comm->cur_analysis_len <= 0) {
        return 0;
    }
    while(main_comm->cur_analysis_len > 1) {
        result = main_comm_check_handle(&main_comm->rcve_buf[rece_idx],main_comm->cur_analysis_len);
        if(result == RECE_DATA_PROCESS_WAIT) {
            rece_start_idx = rece_idx;
            break;
        }
        else if(result == RECE_DATA_PROCESS_FAIL) {
            main_comm->cur_analysis_len -= 1;
            rece_idx += 1;
            continue;
        }
        result = main_comm_data_process(main_comm,&main_comm->rcve_buf[rece_idx],main_comm->cur_analysis_len,&rece_pack_len , usr_config,task,encoder);

        if(result == RECE_DATA_PROCESS_SUCCESS) {
            rece_idx += rece_pack_len;
            main_comm->cur_analysis_len -= rece_pack_len;
            continue;
        }
        else if(result == RECE_DATA_PROCESS_WAIT) {
            rece_start_idx = rece_idx;
            break;
        }
        else {
            rece_idx += 1;
            main_comm->cur_analysis_len -= 1;
            continue;
        }
    }
    if(main_comm->cur_analysis_len > 0){
        memcpy(main_comm->rcve_buf,main_comm->rcve_buf+rece_start_idx,main_comm->cur_analysis_len);
        main_comm->cur_rece_len = main_comm->cur_analysis_len;
    }
    else{
        main_comm->cur_rece_len = 0;
    }
    return 0;
}

int main_comm_data_deal(struct MAIN_COMM_s* main_comm,struct USR_CONFIG_s* usr_config , struct TASK_s* task,struct CONTROLLER_s* controller,struct ENABLE_s *enable)
{
    // if(main_comm->rece_delay_time >= 1000) {
    //     task->statecmd = IDLE;
    //     task->set_state(task,enable,usr_config);
    //     return 0;
    // }
    if(task->statusword_new.errors.errors_code == 0) {
        if(task->fsm.state == IDLE && usr_config->calib_valid == false && task->fsm.state_next != CALIBRATION) {
            task->statecmd = CALIBRATION;
            task->set_state(task,enable,usr_config);
        }
        else if(task->fsm.state == IDLE && task->fsm.state_next != ZERO_CAILBRATION && usr_config->calib_valid == true && usr_config->zero_calib_valid == false) {
            task->statecmd = ZERO_CAILBRATION;
            task->set_state(task,enable,usr_config);
        }
        else if(task->fsm.state == IDLE && task->fsm.state_next != RUN && usr_config->calib_valid == true && usr_config->zero_calib_valid == true) {
            usr_config->control_mode = main_comm->target_cw;
            task->statecmd = RUN;
            task->set_state(task,enable,usr_config);
        }
        else if(task->fsm.state == RUN && task->fsm.state_next != IDLE &&usr_config->calib_valid == true && usr_config->zero_calib_valid == true 
            && usr_config->control_mode != main_comm->target_cw) {
            task->statecmd = IDLE;
            task->set_state(task,enable,usr_config);
        }
    }
    else {
        if(main_comm->error_clear_flg && task->fsm.state == IDLE) {
            task->statecmd = ERRORCLEAR;
            main_comm->error_clear_flg = false;
            task->set_state(task,enable,usr_config);
        }
    }
    
    if(task->fsm.state == RUN) {
        if(usr_config->invert_motor_dir) {
            controller->input_position_buffer = -main_comm->target_pos;
            controller->input_position_buffer = CLAMP(controller->input_position_buffer,-usr_config->position_limit,0);
            controller->input_velocity_buffer = -CLAMP(main_comm->target_vel,-usr_config->velocity_limit,usr_config->velocity_limit);
        }
        else {
            controller->input_position_buffer = CLAMP(main_comm->target_pos,0,usr_config->position_limit);
            controller->input_velocity_buffer = CLAMP(main_comm->target_vel,-usr_config->velocity_limit,usr_config->velocity_limit);
        }
        controller->input_time_buffer = main_comm->control_time;
        controller->sync_callback(controller,usr_config,task);
    }
    return 0;
}




void main_communication_tx_cp(struct MAIN_COMM_s *comm)
{
    comm->tc_flg = true;
    int diff_len = comm->need_send_len - comm->cur_send_len;
    if(diff_len > 0) {
        memcpy(comm->send_buf, &comm->send_buf[comm->cur_send_len], diff_len);
        comm->need_send_len = diff_len;
        comm->cur_send_len = 0;
    }
    else {
        comm->need_send_len = 0;
    }
}

void main_communication_rx_idle(struct MAIN_COMM_s *comm)
{
    if(__HAL_UART_GET_FLAG(comm->usart,UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(comm->usart);
        uint16_t cnt = MAIN_RECE_BUFF_SIZE - __HAL_DMA_GET_COUNTER(comm->usart->hdmarx) - comm->rx_idx;
        if(cnt > ENC_RECE_BUFF_SIZE) {
            comm->cur_rece_len = 0;
            comm->rx_idx = 0;
            return;
        }
        uint16_t all_len = cnt + comm->cur_rece_len;
        if(all_len <= MAIN_RECE_BUFF_SIZE) {
            memcpy(&comm->rcve_buf[comm->cur_rece_len],&comm->usart->pRxBuffPtr[comm->rx_idx],cnt);
            comm->cur_rece_len = all_len;
            comm->rx_idx = MAIN_RECE_BUFF_SIZE - __HAL_DMA_GET_COUNTER(comm->usart->hdmarx);
            comm->rx_flg = true;
            comm->rece_delay_time = 0;
        }
        else {
            //to do 错误码
        }
    }
}

void main_communication_rx_half(struct MAIN_COMM_s *comm)
{
    uint16_t cnt = MAIN_HALF_RECE_BUFF_SIZE - comm->rx_idx;
    uint16_t all_len = cnt + comm->cur_rece_len;
    if(all_len <= MAIN_RECE_BUFF_SIZE){
        memcpy(&comm->rcve_buf[comm->cur_rece_len],&comm->usart->pRxBuffPtr[comm->rx_idx],cnt);
        comm->cur_rece_len = all_len;
        comm->rx_idx = 0;
        comm->rx_flg = true;
        comm->rece_delay_time = 0;
    }
    else{
        //to do 错误码
    }
}

void main_communication_rx_cp(struct MAIN_COMM_s *comm)
{
    uint16_t cnt = MAIN_RECE_BUFF_SIZE - comm->rx_idx;
    uint16_t all_len = cnt + comm->cur_rece_len;
    if(all_len <= MAIN_RECE_BUFF_SIZE){
        memcpy(&comm->rcve_buf[comm->cur_rece_len],&comm->usart->pRxBuffPtr[comm->rx_idx],cnt);
        comm->cur_rece_len = all_len;
        comm->rx_idx = 0;
        comm->rx_flg = true;
        comm->rece_delay_time = 0;
    }
    else{
        //to do 错误码
    }
}

void main_communication_loop(struct MAIN_COMM_s *comm , struct USR_CONFIG_s *config, struct TASK_s *task,struct ENCODER_s* encoder , struct CONTROLLER_s* controller,struct ENABLE_s *enable)
{
    comm->rece_delay_time++;
    if(comm->rece_delay_time > 1000) {
        comm->rece_delay_time = 1000;
    }
    main_comm_rece(comm,config,task,encoder);
    main_comm_data_deal(comm,config,task,controller,enable);
}
void main_communication_init(struct MAIN_COMM_s *comm)
{
    comm->cur_send_len = 0;
    comm->tc_flg = false;
    comm->rx_flg = false;
    comm->rx_idx = 0;
    comm->cur_send_len = 0;
    comm->need_send_len = 0;
    comm->cur_rece_len = 0;
    comm->cur_analysis_len = 0;

    comm->target_cw = CONTROL_MODE_POSITION_FILTER;
    comm->target_pos = 0;
    comm->target_vel = 0;
    comm->control_time = 0;

    comm->error_clear_flg = false;

}

