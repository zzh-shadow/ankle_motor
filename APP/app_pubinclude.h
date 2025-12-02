#ifndef __APP_PUBINCLUDE_H__
#define __APP_PUBINCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "main.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"
#include "dwt.h"
#include "util.h" 

#include "bsp_adc.h"
#include "bsp_tim.h"
#include "bsp_gpio.h"
#include "bsp_usart.h"

#include "calibration.h"
#include "zero_cailbration.h"
#include "controller.h"
#include "encoder.h"
#include "foc.h"
#include "open_loop.h"
#include "task.h"
#include "trap_traj.h"
#include "usr_config.h"
#include "battery.h"
#include "main_communication.h"



#define NUM_MOTORS 1

#define OFFSET_LUT_NUM         128U

#define BATTERY_NUM     5 //电池串数


#define MAX_MOTOR_POLE_PAIRS    5U
#define SAMPLES_PER_PPAIR       128U

#define RECE_DATA_PROCESS_FAIL       0
#define RECE_DATA_PROCESS_SUCCESS    1
#define RECE_DATA_PROCESS_WAIT       2

#define SWAP_16(x) (((x) << 8) | ((x) >> 8))
#define SWAP_32(x) (((x) << 24) | (((x) << 8) & 0x00FF0000) | \
                   (((x) >> 8) & 0x0000FF00) | ((x) >> 24))
#define SWAP_64(x) ((uint64_t)SWAP_32((x) >> 32) | \
                   ((uint64_t)SWAP_32((x) & 0xFFFFFFFF) << 32))



static inline float read_vbus(void) {
    // return (float)(((uint16_t)ADC1->JDR3)) * VBUS_CONVERSION_FACTOR;
    return 24.0f;
} 

typedef enum {
    BOOT_UP            = 0,
    IDLE               = 1,
    RUN                = 2,
    CALIBRATION        = 3,
    ZERO_CAILBRATION   = 4,
    OPEN_LOOP_RUN      = 5,
    ERRORCLEAR         = 6,
} FSM_STATE_e;

typedef struct PWM_GEN_s{
	TIM_HandleTypeDef* motor_tim;

	void (*set_a_duty)(struct PWM_GEN_s* pwm_gen,uint32_t duty);
	void (*set_b_duty)(struct PWM_GEN_s* pwm_gen,uint32_t duty);
	void (*set_c_duty)(struct PWM_GEN_s* pwm_gen,uint32_t duty);
	void (*switch_on_pwm)(struct PWM_GEN_s* pwm_gen);
	void (*switch_off_pwm)(struct PWM_GEN_s* pwm_gen);
	void (*turn_on_low_sides)(struct PWM_GEN_s* pwm_gen);		
}PWM_GEN_t;

typedef struct ADC_SENSOR_s{
    ADC_HandleTypeDef *ADC_a;
    ADC_HandleTypeDef *ADC_b;
    ADC_HandleTypeDef *ADC_c;

    int phase_a_adc_offest;
    int phase_b_adc_offset;
	int phase_c_adc_offset;	

    float (*read_iphase_a)(struct ADC_SENSOR_s* ADCSensor);
    float (*read_iphase_b)(struct ADC_SENSOR_s* ADCSensor);
    float (*read_iphase_c)(struct ADC_SENSOR_s* ADCSensor);
	int (*current_reading_polarization)(struct ADC_SENSOR_s* ADCSensor,struct PWM_GEN_s* PWMGen);	
    
}ADC_SENSOR_t;


typedef struct USR_CONFIG_s{
    int32_t invert_motor_dir;           //运动方向
    float inertia;                      //转动惯量
    float torque_constant;              //力矩常数
    float motor_pole_pairs;             //电机极对数
    float motor_phase_resistance;       //电机相电阻
    float motor_phase_inductance;       //电机相电感
    float motor_psi_f;                  //电机磁链常数    
    float current_limit;                //电流限制
    float velocity_limit;               //速度限制
    float position_limit;              //位置限制

    float cailb_current;                //校准电流
    float calib_voltage;                //校准电压

    int32_t control_mode;               //控制模式
    float pos_gain;                     //位置增益
    float vel_gain;                     //速度增益
    float vel_integrator_gain;          //速度积分增益
    float current_ctrl_bw;              //电流控制带宽   
    int32_t sync_target_enable;         //同步目标使能标志位
    float target_velcity_window;        //目标速度允许窗口
    float target_position_window;       //目标位置允许窗口
    float torque_ramp_rate;             //力矩斜率
    float velocity_ramp_rate;           //速度斜率

    float position_filter_bw;           //位置模式滤波带宽

    float profile_velocity;             //规划器速度限制
    float profile_accel;                //规划器加速度限制
    float profile_decel;                //规划器减速度限制

    float protect_under_voltage;        //欠压保护值
    float protect_over_voltage;         //过压保护值
    float protect_over_current;         //过流保护值
    float protect_i_bus_max;            //母线电流保护值
    float protect_i_leak_max;           //漏电流保护值
    float protect_temp_mcu_high;        //MCU高温保护值
    float protect_temp_mcu_low;         //MCU低温保护值
    float protect_temp_ntcdrive_high;   //驱动板高温保护值
    float protect_temp_ntcdrive_low;    //驱动板低温保护值
    float protect_temp_ntcmotor_high;   //电机高温保护值z   
    float protect_temp_ntcmotor_low;    //电机低温保护值


    uint32_t enc_baudrate;
    uint32_t bat_baudrate;
    uint32_t main_baudrate;


    int32_t calib_valid;            //校准标志位
    int32_t zero_calib_valid;       //零点校准标志位
    int32_t encoder_dir;
    int32_t encoder_offset;
    float encoder_ele_offest;
    float encoder_gr;
    int32_t offset_lut[OFFSET_LUT_NUM];
    int32_t encoder_cnt_limit;

    
}USR_CONFIG_t;


typedef struct CONFIG_API_s{
	void(*usr_config_set_default_config)(struct USR_CONFIG_s* UsrConfig);
	int(*usr_config_read_config)(struct USR_CONFIG_s* UsrConfig);
	int(*usr_config_write_config)(struct USR_CONFIG_s* UsrConfig);	
    int(*usr_config_set_control_mode)(struct USR_CONFIG_s* UsrConfig,uint8_t mode);
}CONFIG_API_t;

typedef enum {
    CS_NULL = 0,

    CS_DIR_PP_START,
    CS_DIR_PP_LOOP,
    CS_DIR_PP_CW_LOOP,
    CS_DIR_PP_END,

    CS_ENCODER_START,
    CS_ENCODER_CW_LOOP,
    CS_ENCODER_CCW_LOOP,
    CS_ENCODER_END,
    CS_ENCODER_CHECK,

    CS_REPORT_OFFSET_LUT,

}CAILB_STEP_e;

typedef struct FSM_s {
    FSM_STATE_e state;
    FSM_STATE_e state_next;
    uint8_t state_next_ready;
} FSM_t;


typedef struct MOTOR_STATUS_s {
    union{
        uint32_t status_code;
        struct{
            uint32_t ready_to_switched_on   : 1;
            uint32_t operation_enabled      : 1;
            uint32_t quick_stop             : 1;
            uint32_t target_reached         : 1;
            uint32_t current_limit_active   : 1;
            uint32_t traj_status            : 1;
            uint32_t PADDING                : 26;
        };
    }status;

    union{
        uint32_t errors_code;
        struct{
            uint32_t adc_selftest_fatal  : 1;
            uint32_t encoder_offline     : 1;
            uint32_t drv_offline         : 1;
            uint32_t encode_error        : 1;

            uint32_t over_voltage        : 1;
            uint32_t under_voltage       : 1;
            uint32_t over_leakcurrent    : 1;
            uint32_t over_phasecurrent   : 1;

            uint32_t over_buscurrent     : 1;
            uint32_t cailbration         : 1;
            uint32_t zero_cailbration    : 1;
            uint32_t PADDING             : 21;
        };
    }errors;

}MOTOR_STATUS_t;



typedef struct TASK_s {
	GPIO_TypeDef * ledport;
	
	uint16_t  ledpin;
	
	uint16_t tick;
	
    uint32_t tick_100Hz;
	
	uint16_t charge_boot_cat_delay;	

    bool init_flg;
		
	FSM_STATE_e statecmd;
	
	volatile MOTOR_STATUS_t statusword_new;
	volatile MOTOR_STATUS_t statusword_old;
	volatile FSM_t fsm;	

    uint32_t test_error_code;
		
	void (*init)(struct TASK_s* MCTask);
	void (*reset_error)(struct TASK_s* MCTask);
	int (*set_state)(struct TASK_s *task,struct ENABLE_s *enable,struct USR_CONFIG_s *usrconfig);
	void (*enter_state)(struct MOTOR_s* Motor);
	void (*exit_state)(struct MOTOR_s* Motor);
	void (*high_frequency_task)(struct MOTOR_s* Motor);
	void (*safety_task)(struct TASK_s* MCTask, struct USR_CONFIG_s* UsrConfig,struct FOC_s* Foc);
	void (*low_priority_task)(struct MOTOR_s* Motor);		
	
} TASK_t;

typedef struct MAIN_COMM_s {
    UART_HandleTypeDef *usart;

    uint8_t rcve_buf[MAIN_RECE_BUFF_SIZE];
    uint8_t send_buf[MAIN_SEND_BUFF_SIZE];

    bool tc_flg;
    bool rx_flg;

    uint32_t cur_send_len;
    uint32_t need_send_len;

    uint32_t cur_rece_len;
    uint32_t cur_analysis_len;

    uint32_t rx_idx;

    uint16_t target_cw;
    float target_pos;
    float target_vel;
    float control_time;

    uint16_t rece_delay_time;

    bool error_clear_flg;

    void (*init)(struct MAIN_COMM_s* main_comm);
    void (*loop)(struct MAIN_COMM_s *comm , struct USR_CONFIG_s *config, struct TASK_s *task,struct ENCODER_s* encoder , struct CONTROLLER_s* controller,struct ENABLE_s *enable);
    void (*tx_cp)(struct MAIN_COMM_s* main_comm);
    void (*rx_idle)(struct MAIN_COMM_s* main_comm);
    void (*rx_half)(struct MAIN_COMM_s* main_comm);
    void (*rx_cp)(struct MAIN_COMM_s* main_comm);

} MAIN_COMM_t;

typedef struct BATTERY_s {

	UART_HandleTypeDef* usart;
	
    uint8_t rcve_buf[BAT_RECE_BUFF_SIZE];
    uint8_t send_buf[BAT_SEND_BUFF_SIZE];

    bool tc_flg;
    bool rx_flg;

    uint32_t cur_send_len;
    uint32_t need_send_len;

    uint32_t cur_rece_len;
    uint32_t cur_analysis_len;

    uint32_t rx_idx;

    float voltage;
    float current;
    float standard_power;
    float current_power;
    uint16_t charging_time;
    uint16_t protection_status;
    uint16_t cycle_count;
    uint8_t SOH;
    uint8_t RSOC;
    uint8_t FET_status;
    uint8_t battery_num;
    float NTC_temperature;
    
    float monomer_battery[BATTERY_NUM];

    float short_press_time;
    float long_press_time;
    
    uint8_t control_bit;
    uint8_t led4_led3_status;
    uint8_t led2_led1_status;
    

    void (*init)(struct BATTERY_s* battery);
    void (*loop)(struct BATTERY_s* battery);
    void (*rx_cp)(struct BATTERY_s* battery);
    void (*tx_cp)(struct BATTERY_s* battery);
    void (*rx_idle)(struct BATTERY_s* battery);
    void (*rx_half)(struct BATTERY_s* battery);
    int (*read_voltage)(struct BATTERY_s* battery);
    int (*set_press_time)(struct BATTERY_s* battery,uint16_t long_time,uint16_t short_time);
    int (*led_control)(struct BATTERY_s* battery,uint8_t led_control,uint8_t led4_led3_status,uint8_t led2_led1_status);
}BATTERY_t;

typedef struct ENCODER_s {
	
	UART_HandleTypeDef* usart;
	
    uint8_t rcve_buf[ENC_RECE_BUFF_SIZE];
    uint8_t send_buf[ENC_SEND_BUFF_SIZE];

    bool tc_flg;
    bool rx_flg;
    uint16_t rx_delay_count;

    uint32_t cur_send_len;
    uint32_t need_send_len;

    uint32_t cur_rece_len;
    uint32_t cur_analysis_len;

    uint32_t rx_idx;

    uint8_t status_flg;
    uint8_t first_flg;
    int32_t one_pos_count;
    int32_t last_one_pos_count;
    int16_t more_pos_count;

    float time_diff;
    float cur_time;
    float last_time;
    uint8_t time_diff_count;

    float pos_cpr_count;
    float vel_estimate_counts;

    int64_t shadow_count;

    uint32_t check_count;

    int delta_count_record[4];	

    float pos;
    float vel;
    
    float phase;
    float phase_count;
    float phase_vel;
    float ele_phase;

    float pll_kp;
    float pll_ki;
    float interpolation;
    float snap_threshold;
	
	void (*init)(struct ENCODER_s* encoder , struct ENABLE_s* enable);
	void (*loop)(struct ENCODER_s* Encoder,struct USR_CONFIG_s* UsrConfig , struct TASK_s* task);
    int (*cmd)(struct ENCODER_s* encoder , uint8_t cmd);
    void (*tx_cp)(struct ENCODER_s* encoder);
    void (*rx_idle)(struct ENCODER_s* encoder);
    void (*rx_half)(struct ENCODER_s* encoder);
    void (*rx_cp)(struct ENCODER_s* encoder);


} ENCODER_t;


typedef struct CALIBRATION_s{
    uint32_t loop_count;
	float phase_set;
    float start_count;
    float check_count;
	int16_t sample_count;
    int64_t current_diff;
	float next_sample_time;	
    bool calib_dir_flg;

	int error_arr[MAX_MOTOR_POLE_PAIRS*SAMPLES_PER_PPAIR];

    float last_error_lut;
    double error_lut_mov;
    int error_lut_idx;

    uint8_t get_out_count;

	CAILB_STEP_e calib_step;

	void (*calibration_start)(struct CALIBRATION_s* calibration,struct USR_CONFIG_s* UsrConfig);	
	void (*calibration_end)(struct CALIBRATION_s* calibration,struct FOC_s* Foc,struct PWM_GEN_s* PWMGen);	
	void (*calibration_loop)(struct MOTOR_s* Motor);	

}CALIBRATION_t;


typedef struct ZERO_CAILBRATION_s{
    uint16_t current_limit_count;
    uint16_t cail_time_count;

    void (*start)(struct ZERO_CAILBRATION_s *zero_cail,struct USR_CONFIG_s *usr_config);
    void (*end)(struct ZERO_CAILBRATION_s *zero_cail,struct CONTROLLER_s *controller,struct USR_CONFIG_s *usr_config,struct ENCODER_s* encoder);
    void (*loop)(struct MOTOR_s *motor);
}ZERO_CAILBRATION_t;

typedef struct OPEN_LOOP_s{
    float voltage;
    float phase_set;


    void (*start)(struct OPEN_LOOP_s *open_loop ,struct FOC_s *foc, struct PWM_GEN_s *pwm_gen ,struct USR_CONFIG_s *usr_config);
    void (*end)(struct OPEN_LOOP_s *open_loop , struct FOC_s *foc , struct PWM_GEN_s *pwm_gen);
    void (*loop)(struct MOTOR_s *motor);
}OPEN_LOOP_t;

typedef struct TRAJ_s {

    float cur_accel;
    float cur_decel;

    float accel_time;
    float decel_time;
    
    float input_pos;
    float vel_set;  

    float vel_set_point;
    float pos_set_point;

    float distance;
    float acc_distance;
    float keep_distance;
    float dec_distance;

    float acc_distance_inc;
    float keep_distance_inc;
    float dec_distance_inc;

    float keep_time;

    float peak_vel;


    int dir_flg;
    bool stop_flg;

    int (*plan)(struct TRAJ_s* traj,float t_pos,float s_pos, float t_vel, float s_vel, float time,float max_accel,float max_dccel,float max_vel);
    void (*eval)(struct TRAJ_s* traj,struct ENCODER_s* encoder,struct CONTROLLER_s* controller,struct USR_CONFIG_s *usr_config);
}TRAJ_t;



typedef struct CONTROLLER_s{
    float input_position;
    float input_velocity;
    float input_torque;
    float input_time;
    
    float input_position_buffer;
    float input_velocity_buffer;
    float input_torque_buffer;
    float input_time_buffer;
    
    float pos_setpoint;
    float vel_setpoint;
    float torque_setpoint;


    bool input_updated;
    float input_pos_filter_kp;
    float input_pos_filter_ki;
    float vel_integrator_torque;

    float v_err;
	float vel_des;
	float pos_meas ;

    float torque;
    float torque_limit;
    float i_q_set;



	int (*set_home)(struct CONTROLLER_s* Controller,struct ENCODER_s* Encoder,struct TASK_s* MCTask,struct TRAJ_s* Traj);
	void(*sync_callback)(struct CONTROLLER_s* Controller,struct USR_CONFIG_s* UsrConfig,struct TASK_s* MCTask);
	void (*init)(struct CONTROLLER_s* Controller,struct USR_CONFIG_s* UsrConfig);
	void (*update_input_pos_filter_gain)(struct CONTROLLER_s* Controller,float bw);
	void (*reset)(struct CONTROLLER_s* Controller,struct ENCODER_s* Encoder,struct TRAJ_s* Traj,struct USR_CONFIG_s* UsrConfig);
	void (*loop)(struct MOTOR_s* Motor);
}CONTROLLER_t;


typedef struct ENABLE_s{

    GPIO_TypeDef* motor_enable_gpio;
    GPIO_TypeDef* motor_boost_gpio;
    GPIO_TypeDef* encoder_enable_gpio;
    GPIO_TypeDef* test_gpio;

    uint16_t motor_drive_pin;
    uint16_t motor_boost_pin;    
    uint16_t encoder_enable_pin;
    uint16_t test_pin;

    void (*drive_enable)(struct ENABLE_s *enable ,bool is_enable);
    void (*boost_enable)(struct ENABLE_s *enable ,bool is_enable);
    void (*encoder_enable)(struct ENABLE_s *enable ,bool is_enable);
    void (*test_enable)(struct ENABLE_s *enable ,bool is_enable);

}ENABLE_t;


 typedef struct FOC_s {
    bool is_armed;
    
    float v_bus, v_bus_filt, i_a, i_b, i_c,i_sq,i_sq_filt;

    float i_q, i_q_filt, i_d, i_d_filt, i_bus, i_bus_filt, power_filt;
    float dtc_a, dtc_b, dtc_c;
    
    float current_ctrl_p_gain, current_ctrl_i_gain;
    float current_ctrl_integral_d, current_ctrl_integral_q;

    uint32_t time_count;
    uint32_t torque_time_count;

    uint32_t over_current_time;

    bool check_flg;
    uint16_t adc_offest_idx;

    float mod_alpha;
    float mod_beta;
	void (*init)(struct FOC_s* Foc,struct USR_CONFIG_s* UsrConfig);
    void (*update_current_ctrl_gain)(struct FOC_s* Foc, struct USR_CONFIG_s* UsrConfig);
    void (*arm)(struct FOC_s* Foc,struct ENABLE_s* enable, struct PWM_GEN_s* PWMGen);
    void (*disarm)(struct FOC_s* Foc,struct ENABLE_s* enable ,struct PWM_GEN_s* PWMGen);
    void (*voltage)(struct FOC_s* Foc, float Vd_set, float Vq_set, float phase,struct PWM_GEN_s* PWMGen);
    void (*current)(struct FOC_s* Foc, float Id_set, float Iq_set, float phase, float phase_vel,
            struct PWM_GEN_s* PWMGen,struct TASK_s* task,struct USR_CONFIG_s* usr_config,struct ENCODER_s* encoder);	
	
} FOC_t;




typedef struct MOTOR_s{
	uint8_t       debug_state;
	uint8_t       debug_flag;
    
	ADC_SENSOR_t    adc_sensor; 	
	PWM_GEN_t  	    pwm_gen;
	CALIBRATION_t   calibration;
    ZERO_CAILBRATION_t zero_cailbration;
	CONTROLLER_t	controller;
	ENCODER_t       encoder;
    BATTERY_t       battery;
    MAIN_COMM_t     main_comm;
	FOC_t 	        foc; 
    ENABLE_t        enable;
	OPEN_LOOP_t     open_loop;
	TASK_t          task;
    TRAJ_t          traj;
	USR_CONFIG_t    usrconfig;
	CONFIG_API_t    usrconfigapi;	
} MOTOR_t;



#ifdef __cplusplus
}
#endif


#endif
