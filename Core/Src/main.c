/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms· that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
volatile uint64_t TasktickCount = 0;
volatile uint32_t SystickCount = 0;
volatile uint64_t App_period_1ms=0;
volatile uint64_t App_period_5ms=0;
volatile uint64_t App_period_1s=0;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
MOTOR_t motor[NUM_MOTORS] = {
  {
    .adc_sensor = {
      .ADC_a = &hadc1, .ADC_b = &hadc1, .ADC_c = &hadc1, .current_reading_polarization = pwmc_current_reading_polarization, .read_iphase_a = bsp_read_iphase_a,
      .read_iphase_b = bsp_read_iphase_b,.read_iphase_c = bsp_read_iphase_c
    },
    .calibration = {
      .calib_step = CS_NULL, .calibration_start = calibration_start, .calibration_end = calibration_end, .calibration_loop = calibration_loop
    },
    .zero_cailbration = {
      .start = zero_cailbration_start, .end = zero_cailbration_stop, .loop = zero_cailbration_loop
    },
    .controller = {
      .set_home = controller_set_home, .sync_callback = controller_sync_callback, .init = controller_init, .reset = controller_reset,
      .loop = controller_loop
    },
    .traj = {
      .plan = traj_plan, .eval = traj_eval
    },
    .encoder = {
      .usart = &huart2, .init = encoder_init, .loop = encoder_loop, .tx_cp = encoder_tx_cp, .cmd = encoder_cmd,
      .rx_idle = encoder_rx_idle, .rx_half = encoder_rx_half, .rx_cp = encoder_rx_cp
    },
    .battery = {
      .usart = &huart3, .init = battery_init, .loop = battery_loop, .tx_cp = battery_tx_cp, .rx_idle = battery_rx_idle,
      .rx_half = battery_rx_half, .rx_cp = battery_rx_cp,.read_voltage = battery_read_voltage,.set_press_time = battery_set_press_time, .led_control = battery_led_control
    },
    .main_comm = {
      .usart = &huart1, .init = main_communication_init, .loop = main_communication_loop, .tx_cp = main_communication_tx_cp,
      .rx_idle = main_communication_rx_idle, .rx_half = main_communication_rx_half, .rx_cp = main_communication_rx_cp
    },
    .enable = {
      .motor_enable_gpio = GPIOC, .encoder_enable_gpio = GPIOC, .motor_boost_gpio = GPIOC,.test_gpio = GPIOA,.test_pin = GPIO_PIN_12,.motor_boost_pin = GPIO_PIN_8 ,.motor_drive_pin = GPIO_PIN_13 ,.encoder_enable_pin = GPIO_PIN_7,
      .drive_enable = enable_motor_drive, .boost_enable = enable_boost, .encoder_enable = enable_encoder, .test_enable = test_gpio
    },
    .pwm_gen = {
      .motor_tim = &htim1, .set_a_duty = set_a_duty, .set_b_duty = set_b_duty, .set_c_duty = set_c_duty, .switch_off_pwm = switch_off_pwm, .switch_on_pwm = switch_on_pwm,
      .turn_on_low_sides = turn_on_low_sides_pwm
    },
    .foc = {
      .init = FOC_init, .update_current_ctrl_gain = FOC_update_current_ctrl_gain, .arm = FOC_arm, .disarm = FOC_disarm, .voltage = FOC_voltage, .current = FOC_current
    },
    .open_loop = {
      .start = open_loop_start, .end = open_loop_end, .loop = open_loop_loop
    },
    .task = {
      .init = task_init, .reset_error = task_reset_error, .set_state = task_set_state, .enter_state = task_enter_state, .exit_state = task_exit_state,
      .high_frequency_task = task_high_frequency_task, .safety_task = task_safety_task, .low_priority_task = task_low_priority_task
    },
    .usrconfigapi = {
      .usr_config_set_default_config = usr_config_set_default_config,
      .usr_config_write_config = usr_config_write_config,
      .usr_config_read_config = usr_config_read_config,
      .usr_config_set_control_mode = usr_config_set_control_mode
    },
  },
};

void sys_tick_call_back(void) 
{
  if(TasktickCount - App_period_1ms >= 1){
    App_period_1ms = TasktickCount;
    for(int i = 0;i < NUM_MOTORS;i++){
      if(motor[i].debug_flag) {
        motor[i].debug_flag = 0;

        motor[i].task.statecmd = motor[i].debug_state;
        motor[i].task.set_state(&motor[0].task,&motor[0].enable,&motor[0].usrconfig);
        motor[i].controller.sync_callback(&motor[i].controller,&motor[i].usrconfig,&motor[i].task);
      }
      motor[i].task.low_priority_task(&motor[i]);
    }
  }

  if(TasktickCount - App_period_5ms >= 5){
    App_period_5ms = TasktickCount;
  }

  if(TasktickCount - App_period_1s >= 1000) {
    App_period_1s = TasktickCount;
    // motor[0].battery.loop(&motor->battery);
    if(motor[0].task.fsm.state == BOOT_UP) {
      motor[0].task.statecmd = IDLE;
      motor[0].usrconfig.control_mode = 2;
      motor[0].task.set_state(&motor[0].task,&motor[0].enable,&motor[0].usrconfig);
      motor[0].controller.sync_callback(&motor[0].controller,&motor[0].usrconfig,&motor[0].task);
    }
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	DWT_Tick_TimerInit();

  HAL_Delay(500);

  for(int i = 0;i < NUM_MOTORS;i++){
    motor[i].task.init(&motor[i].task);
    motor[i].usrconfigapi.usr_config_set_default_config(&motor[i].usrconfig);
  }

  bsp_adc_init();
  bsp_usart_init();

  for(int i = 0; i < NUM_MOTORS; i++){
    motor[i].foc.init(&motor[i].foc , &motor[i].usrconfig);
    motor[i].encoder.init(&motor[i].encoder,&motor[i].enable);
    motor[i].main_comm.init(&motor[i].main_comm);
    motor[i].battery.init(&motor[i].battery);
    motor[i].controller.init(&motor[i].controller,&motor[i].usrconfig);
  }

  bsp_tim_init();

  for(int i = 0; i < NUM_MOTORS; i++){
    if(motor[i].adc_sensor.current_reading_polarization(&motor[i].adc_sensor,&motor[i].pwm_gen) != 0){
      motor[i].task.statusword_new.errors.adc_selftest_fatal = 1;

      motor[i].task.statecmd = IDLE;
      motor[i].task.set_state(&motor[0].task,&motor[0].enable,&motor[0].usrconfig);
      motor[i].task.init_flg = true;
    }
  }
  // motor[0].traj.plan(&motor[0].traj,test_t_pos,0,test_t_vel,0,test_time,test_t_acc,test_t_dec,45);
  

  // switch_on_pwm(&motor[0].pwm_gen);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
