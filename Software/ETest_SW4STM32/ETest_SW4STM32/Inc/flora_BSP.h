#ifndef _FLORA_BSP
#define _FLORA_BSP
#include "main.h"
#include "DS3231.h"
#include "stm32f0xx_hal.h"
#include "gdefine.h"

/***** Quadrature Encoder *****/
// #ATTN: LP Period modified from 300 to 200 in this version
#define KDN_LONG_PRESS_PERIOD	200
#define KDN_CANCEL_PERIOD		600
// The data structure that shall be passed to MFA_CALL
// Data Structure: | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//                                  LP  KDN ROT FRN
#define QINF_BIT_FW_RVN			0
#define QINF_BIT_ROT			1
#define QINF_BIT_KDN			2
#define QINF_BIT_LONG_PRESS		3


/***** DISPLAY PROPERTIES *****/
#define IV9_PULSE_MAX 479
#define IV9_DECAY_TMAX 100
#define IV9_FADE_TMAX 10
#define RGB_PULSE_MAX 470


/***** IV9 DRIVER FUNCTIONS *****/
void IV9_send(uint8_t* a_buf, uint8_t length, uint8_t forceDp);

/***** IV9 EFFECT FUNCTIONS *****/
void IV9_FastPWMSet(TIM_HandleTypeDef* htim, uint8_t ch, uint32_t value);
void IV9_QuadDecay(uint32_t* pulse_buf);
void IV9_LinFadeOut(uint32_t* pulse_buf, uint8_t unit);
void IV9_LinFadeIn(uint32_t* pulse_buf, uint8_t unit);
#define IV9_MixDimming(SRC, CH) ((SRC[CH]*\
		g_scheme_reg[g_curScheme].IV9_pValue[CH])/IV9_PULSE_MAX)

#define IV9_MixDimming_Div(DIV, SRC, CH) (((SRC[CH]*\
		g_scheme_reg[g_curScheme].IV9_pValue[CH])/IV9_PULSE_MAX)>>DIV)

#define RGB_MixDimming_D(SRC) \
	((SRC)*g_scheme_reg[g_curScheme].LED_pValue.R)/RGB_PULSE_MAX, \
	((SRC)*g_scheme_reg[g_curScheme].LED_pValue.G)/RGB_PULSE_MAX, \
	((SRC)*g_scheme_reg[g_curScheme].LED_pValue.B)/RGB_PULSE_MAX


/***** RGB LED Functions *****/
void RGB_FastPWMSet(TIM_HandleTypeDef* htim, RGB_t pulseValue);
void RGB_FastPWMSet_D(TIM_HandleTypeDef* htim,\
		uint16_t R, uint16_t G, uint16_t B);

/***** DS3231 FUNCTIONS *****/
HAL_StatusTypeDef DS3231_CheckBusy(I2C_HandleTypeDef* hi2c, uint32_t Timeout);
HAL_StatusTypeDef DS3231_Config(I2C_HandleTypeDef* hi2c,\
		DS3231_control_t* ctrl_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_GetConfig(I2C_HandleTypeDef* hi2c,\
		DS3231_control_t* ctrl_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_SetAging(I2C_HandleTypeDef* hi2c,\
		DS3231_control_t* ctrl_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_GetAging(I2C_HandleTypeDef* hi2c,\
		DS3231_control_t* ctrl_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_GetTemp(I2C_HandleTypeDef* hi2c,\
		DS3231_control_t* ctrl_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_GetTime(I2C_HandleTypeDef* hi2c,\
		DS3231_time_t* timest_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_SetTime(I2C_HandleTypeDef* hi2c,\
		DS3231_time_t* timest_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_GetTime_Partial(I2C_HandleTypeDef* hi2c, DS3231_TSECT_E section,\
		DS3231_time_t* timest_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_SetTime_Partial(I2C_HandleTypeDef* hi2c, DS3231_TSECT_E section,\
		DS3231_time_t* timest_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_WriteAlarm(I2C_HandleTypeDef* hi2c, uint8_t ch,\
		DS3231_alarm_t* alrmst_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_ReadAlarm(I2C_HandleTypeDef* hi2c, uint8_t ch,\
		DS3231_alarm_t* alrmst_p, uint32_t Timeout);
int DS3231_CheckAlarm(I2C_HandleTypeDef* hi2c, uint8_t ch, uint32_t Timeout);

void DS3231_ExtractBCD (uint8_t* dest, uint8_t input, uint8_t lead);
void DS3231_PackBCD (uint8_t* dest, uint8_t input_h, uint8_t input_l);

uint8_t GetWeekDay(uint8_t y,uint8_t m, uint8_t d);

/***** FLASH FUNCTIONS *****/
/*
HAL_StatusTypeDef FLASH_Fuck(uint16_t* data_halfword, int size);
*/

#endif /* _FLORA_BSP */
