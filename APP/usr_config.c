#include "usr_config.h"

void usr_config_set_default_config(struct USR_CONFIG_s *usr_config)
{
    usr_config->cailb_current = 1.0f;           //校准电流
    usr_config->calib_voltage = 2.4f;           //校准电压
    usr_config->calib_valid = false;                //校准标志位
    usr_config->zero_calib_valid = false;                //零点校准标志位

    usr_config->encoder_gr = 1.0f;              //减速比
    usr_config->encoder_cnt_limit = 524288;    //编码器错误检测计数限制

    usr_config->invert_motor_dir = 1;           //运动方向
    usr_config->inertia = 0.0f;                 //转动惯量
    usr_config->torque_constant = 0.0601f;         //力矩常数 //恒立 0.057 捷昌 0.0601
    usr_config->motor_pole_pairs = 5;           //电机极对数 //恒立 4 捷昌 5
    usr_config->motor_psi_f = 0.01f;                //电机磁链
    usr_config->motor_phase_resistance = 0.43f;     //电机相电阻 //恒立 0.8 捷昌 0.43
    usr_config->motor_phase_inductance = 0.000115f;     //电机相电感//恒立 0.00065 捷昌 0.000115

    usr_config->current_limit = 5.0f;           //电流限制
    usr_config->velocity_limit = 50.0f;          //速度限制
    usr_config->position_limit = 12.5f;         //位置限制

    usr_config->control_mode = CONTROL_MODE_VELOCITY_RAMP;               //控制模式
    usr_config->pos_gain = 10.0f;                //位置增益  //恒立       捷昌 5.0f
    usr_config->vel_gain = 0.05f;                //速度增益 //恒立       捷昌 0.05f
    usr_config->vel_integrator_gain = 0.5f;     //速度积分增益 //恒立       捷昌 0.5f
    usr_config->current_ctrl_bw = 100.0f;            //电流控制带宽   
    usr_config->sync_target_enable = 0  ;         //同步目标使能标志位
    usr_config->encoder_offset = 0;            //编码器偏移量
    usr_config->encoder_ele_offest = 0.0f;

    usr_config->target_velcity_window = 0.5f;   //目标速度允许窗口
    usr_config->target_position_window = 0.1f;  //目标位置允许窗口
    usr_config->torque_ramp_rate = 10.0f;       //力矩斜率
    usr_config->velocity_ramp_rate = 100.0f;    //速度斜率
    usr_config->position_filter_bw = 200.0f;        //位置模式滤波带宽
    
    usr_config->profile_velocity = 50/usr_config->encoder_gr;     //规划器速度限制
    usr_config->profile_accel = 100/usr_config->encoder_gr;        //规划器加速度限制
    usr_config->profile_decel = 100/usr_config->encoder_gr;        //规划器减速度限制

    usr_config->protect_under_voltage = 18;      //欠压保护值
    usr_config->protect_over_voltage = 27;       //过压保护值
    usr_config->protect_over_current = SQ(8.4);       //过流保护值
    usr_config->protect_i_bus_max = 1.0;          //母线电流保护值
    usr_config->protect_i_leak_max = 1.0;         //漏电流保护值

    usr_config->protect_temp_mcu_high = 65.0f;      //MCU高温保护值
    usr_config->protect_temp_mcu_low = 0.0f;       //MCU低温保护值
    usr_config->protect_temp_ntcdrive_high = 105.0f;    //驱动板高温保护值
    usr_config->protect_temp_ntcdrive_low = 0.0f;     //驱动板低温保护值
    usr_config->protect_temp_ntcmotor_high = 85.0f;      //电机高温保护值
    usr_config->protect_temp_ntcmotor_low = 0.0f;       //电机低温保护值
}

int USR_CONFIG_erease_config(void)
{
    return 0;
}

int usr_config_read_config(struct USR_CONFIG_s *usr_config)
{
    return 0;
}

int usr_config_write_config(struct USR_CONFIG_s *usr_config)
{
    return 0;
}

int usr_config_set_control_mode(struct USR_CONFIG_s *usr_config , uint8_t mode)
{
    usr_config->control_mode = mode;
    return 0;
}
