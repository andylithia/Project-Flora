#ifndef _FLORA_BSP
#define _FLORA_BSP
#include "main.h"
#include "DS3231.h"
#include "stm32f0xx_hal.h"

#define IV9_PULSE_MAX 479
#define IV9_DECAY_TMAX 100
#define IV9_FADE_TMAX 10

/***** IV9 DRIVER FUNCTIONS *****/
void IV9_send(uint8_t* a_buf, uint8_t length);

/***** IV9 EFFECT FUNCTIONS *****/
void IV9_FastPWMSet(TIM_HandleTypeDef* htim, uint8_t ch, uint32_t value);
void IV9_QuadDecay(uint32_t* pulse_buf);
inline void IV9_LinFadeOut(uint32_t* pulse_buf, uint8_t unit);
inline void IV9_LinFadeIn(uint32_t* pulse_buf, uint8_t unit);

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

inline void DS3231_ExtractBCD (uint8_t* dest, uint8_t input, uint8_t lead);
inline void DS3231_PackBCD (uint8_t* dest, uint8_t input_h, uint8_t input_l);

#endif /* _FLORA_BSP */
