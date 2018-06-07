
#include "flora_BSP.h"
#include "main.h"
#include "stm32f0xx_hal.h"
#include "DS3231.h"

// extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
// extern DMA_HandleTypeDef hdma_spi1_tx;
// extern TIM_HandleTypeDef htim1;
// extern TIM_HandleTypeDef htim2;
// extern TIM_HandleTypeDef htim14;
// extern TIM_HandleTypeDef htim16;
// extern TIM_HandleTypeDef htim17;

uint8_t IV9_font_LUT[] = {
	0x00, 0x60, 0x21, 0x3f, 0x95, 0x2b, 0x17, 0x20,
	0x83, 0x34, 0xa9, 0x38, 0x14, 0x08, 0x40, 0x2a,
	0xb7, 0x30, 0xae, 0xbc, 0x39, 0x9d, 0x9f, 0xb0,
	0xbf, 0xbd, 0x84, 0x94, 0x09, 0x0c, 0x28, 0xe0,
	0xaf, 0xbe, 0x1f, 0x0e, 0x3e, 0x8f, 0x8b, 0x97,
	0x1b, 0x10, 0x34, 0x9b, 0x07, 0x93, 0x1a, 0x1e,
	0xab, 0xb9, 0x0a, 0x9c, 0x0f, 0x37, 0x16, 0x27,
	0x3b, 0x3d, 0x8e, 0x87, 0x19, 0xb4, 0xa1, 0x04 };

void IV9_send(uint8_t* a_buf, uint8_t length){
	uint8_t buffer[6];
	for(int i=0;i<length;i++)
		buffer[length-i-1] = IV9_font_LUT[(a_buf[i]-' ')];
	HAL_SPI_Transmit(&hspi1,buffer,length,0xFF);
	HAL_GPIO_WritePin(IV9LATCH_GPIO_Port, IV9LATCH_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(IV9LATCH_GPIO_Port, IV9LATCH_Pin, GPIO_PIN_RESET);
}
/**
  * @brief  Push new pulse value into the PWM channels.
  * @param  htim TIM handle
  * @param  channels (single-hot encoding)
  * @param	pulse value
  * @retval Nothing
*/
void IV9_FastPWMSet(TIM_HandleTypeDef* htim, uint8_t ch_sh, uint32_t value){
	if(ch_sh&BIT(0)){
		htim->Instance->CCER &= ~TIM_CCER_CC1E; // Disable the ch1
		htim->Instance->CCR1 = value;
	}
	if(ch_sh&BIT(1)){
		htim->Instance->CCER &= ~TIM_CCER_CC2E; // Disable the ch2
		htim->Instance->CCR2 = value;
	}
	if(ch_sh&BIT(2)){
		htim->Instance->CCER &= ~TIM_CCER_CC3E; // Disable the ch3
		htim->Instance->CCR3 = value;
	}
	if(ch_sh&BIT(3)){
		htim->Instance->CCER &= ~TIM_CCER_CC4E; // Disable the ch4
		htim->Instance->CCR4 = value;
	}
	htim->Instance->CCER |= ch_sh;				// Re-enable the chs
}


/**
  * @brief  Perform an approximation of log decay on a PULSE register (479)
  * @param  pointer towards the target register
  * @retval Nothing
*/
void IV9_QuadDecay(uint32_t* pulse_buf){
		if(*pulse_buf>440)		(*pulse_buf)-=2; // -40
		else if(*pulse_buf>240)	(*pulse_buf)-=8; // -40
		else if(*pulse_buf>120)	(*pulse_buf)-=6; // -320
		else if(*pulse_buf>40)	(*pulse_buf)-=2; // -40
		else if(*pulse_buf!=0)	(*pulse_buf)--; // -160
}

/**
  * @brief  Fade out a PULSE register at a constant rate
  * @param  pointer towards the target register
  * @param  fade out rate
  * @retval Nothing
*/
void IV9_LinFadeOut(uint32_t* pulse_buf, uint8_t unit){
	if(*pulse_buf>=unit) (*pulse_buf)-=unit;
}

/**
  * @brief  Fade in a PULSE register at a constant rate
  * @param  pointer towards the target register
  * @param  fade in rate
  * @retval Nothing
*/
void IV9_LinFadeIn(uint32_t* pulse_buf, uint8_t unit){
	if(*pulse_buf<=(IV9_PULSE_MAX-unit)) (*pulse_buf)+=unit;
}

/**
  * @brief  Check whether the DS3231 is busy
  * @param  hi2c I2C Peripheral handle
  * @param  Timeout
  * @retval HAL_StatusTypeDef HAL_OK or HAL_BUSY
*/
HAL_StatusTypeDef DS3231_CheckBusy(I2C_HandleTypeDef* hi2c, uint32_t Timeout){
	uint8_t CTRL2_STAT_r;
	HAL_StatusTypeDef rtn_r;
	rtn_r = HAL_I2C_Mem_Read(hi2c, DS3231_BASE_ADR, DS3231_ADR_CTRL2_STAT,
			I2C_MEMADD_SIZE_8BIT, &CTRL2_STAT_r, 1, Timeout);
	if(rtn_r == HAL_OK)
		if(CTRL2_STAT_r&DS3231_BSY)
			return HAL_BUSY;
	return rtn_r;
}
// NOTE: The busy bit is not examined in the following functions


/**
  * @brief  Configure the DS3231
  * @param  hi2c I2C Peripheral handle
  * @param	DS3231 control register handle
  * @param  Timeout
  * @retval HAL_StatusTypeDef
*/
HAL_StatusTypeDef DS3231_Config(I2C_HandleTypeDef* hi2c,\
		DS3231_control_t* ctrl_p, uint32_t Timeout){
	return HAL_I2C_Mem_Write(hi2c, DS3231_BASE_ADR, DS3231_ADR_CTRL1,
			I2C_MEMADD_SIZE_8BIT, &(ctrl_p->CTRL1), 2, Timeout);
}

/**
  * @brief  Read the DS3231 configure
  * @param  hi2c I2C Peripheral handle
  * @param	DS3231 control register handle
  * @param  Timeout
  * @retval HAL_StatusTypeDef
*/
HAL_StatusTypeDef DS3231_GetConfig(I2C_HandleTypeDef* hi2c,\
		DS3231_control_t* ctrl_p, uint32_t Timeout){
	return HAL_I2C_Mem_Read(hi2c, DS3231_BASE_ADR, DS3231_ADR_CTRL2_STAT,
			I2C_MEMADD_SIZE_8BIT, &(ctrl_p->CTRL2_STAT), 1, Timeout);
}

/**
  * @brief  Read the DS3231 aging settings
  * @param  hi2c I2C Peripheral handle
  * @param	DS3231 control register handle
  * @param  Timeout
  * @retval HAL_StatusTypeDef
*/
HAL_StatusTypeDef DS3231_GetAging(I2C_HandleTypeDef* hi2c,\
		DS3231_control_t* ctrl_p, uint32_t Timeout){
	return HAL_I2C_Mem_Read(hi2c, DS3231_BASE_ADR, DS3231_ADR_AOFFSET,
			I2C_MEMADD_SIZE_8BIT, &(ctrl_p->AOFFSET), 1, Timeout);
}

/**
  * @brief  Read the DS3231 temperature sensor data
  * @param  hi2c I2C Peripheral handle
  * @param	DS3231 control register handle
  * @param  Timeout
  * @retval HAL_StatusTypeDef
*/
HAL_StatusTypeDef DS3231_GetTemp(I2C_HandleTypeDef* hi2c,\
		DS3231_control_t* ctrl_p, uint32_t Timeout){
	return HAL_I2C_Mem_Read(hi2c, DS3231_BASE_ADR, DS3231_ADR_TEMP_H,
			I2C_MEMADD_SIZE_8BIT, &(ctrl_p->TEMP), 2, Timeout);
}

/**
  * @brief  Read time from DS3231
  * @param  hi2c I2C Peripheral handle
  * @param	DS3231 time register handle
  * @param  Timeout
  * @retval HAL_StatusTypeDef
*/
HAL_StatusTypeDef DS3231_GetTime(I2C_HandleTypeDef* hi2c,\
		DS3231_time_t* timest_p, uint32_t Timeout){
	HAL_StatusTypeDef rtn_r;

	/* Read Second & Minutes */
	rtn_r = HAL_I2C_Mem_Read(hi2c, DS3231_BASE_ADR, DS3231_ADR_SEC,
				I2C_MEMADD_SIZE_8BIT, &(timest_p->SEC), 2, Timeout);
	if(rtn_r!=HAL_OK) return rtn_r;

	/*Read and unpack the Hour Register*/
	uint8_t hour_r;
	rtn_r = HAL_I2C_Mem_Read(hi2c, DS3231_BASE_ADR, DS3231_ADR_HOUR,
				I2C_MEMADD_SIZE_8BIT, &(hour_r), 1, Timeout);
	if(rtn_r!=HAL_OK) return rtn_r;
	if(hour_r&BIT(6)){
		/* 12HR */
		timest_p->HOUR.HOUR_TYPE = DS3231_HOUR_TYPE_12;
		timest_p->HOUR.AMn_PM = (hour_r&BIT(5))?1:0;
		timest_p->HOUR.HOUR = hour_r&B8(00011111);
	} else {
		/* 24HR */
		timest_p->HOUR.HOUR_TYPE = DS3231_HOUR_TYPE_24;
		timest_p->HOUR.AMn_PM = 0;
		timest_p->HOUR.HOUR = hour_r&B8(00111111);
	}

	/* Read Day & Date */
	rtn_r = HAL_I2C_Mem_Read(hi2c, DS3231_BASE_ADR, DS3231_ADR_DAY,
				I2C_MEMADD_SIZE_8BIT, &(timest_p->DAY), 2, Timeout);
	if(rtn_r!=HAL_OK) return rtn_r;

	/*Read and unpack the Month / Century Register*/
	uint8_t c_mon_r;
	rtn_r = HAL_I2C_Mem_Read(hi2c, DS3231_BASE_ADR, DS3231_ADR_C_MONTH,
				I2C_MEMADD_SIZE_8BIT, &(c_mon_r), 1, Timeout);
	if(rtn_r!=HAL_OK) return rtn_r;
	timest_p->CENTURY = c_mon_r&BIT(7)?1:0;
	timest_p->MONTH = c_mon_r&B8(00011111);

	return HAL_OK;
}

/**
  * @brief  Write time to DS3231
  * @param  hi2c I2C Peripheral handle
  * @param	DS3231 time register handle
  * @param  Timeout
  * @retval HAL_StatusTypeDef
*/
HAL_StatusTypeDef DS3231_SetTime(I2C_HandleTypeDef* hi2c,\
		DS3231_time_t* timest_p, uint32_t Timeout){
	HAL_StatusTypeDef rtn_r;
	rtn_r = HAL_I2C_Mem_Write(hi2c, DS3231_BASE_ADR, DS3231_ADR_SEC,
				I2C_MEMADD_SIZE_8BIT, &(timest_p->SEC), 2, Timeout);
	if(rtn_r != HAL_OK)
		return rtn_r;

	uint8_t hour_r = 0;
	if(timest_p->HOUR.HOUR_TYPE){
		hour_r |= BIT(6);
		if(timest_p->HOUR.AMn_PM)
			hour_r |= BIT(5);
		hour_r |= (timest_p->HOUR.HOUR.HOUR_12 & B8(00011111));
	} else
		hour_r |= (timest_p->HOUR.HOUR.HOUR_24 & B8(00111111));

	rtn_r = HAL_I2C_Mem_Write(hi2c, DS3231_BASE_ADR, DS3231_ADR_HOUR,
				I2C_MEMADD_SIZE_8BIT, &(hour_r), 1, Timeout);
	if(rtn_r != HAL_OK)
		return rtn_r;

	rtn_r = HAL_I2C_Mem_Write(hi2c, DS3231_BASE_ADR, DS3231_ADR_DAY,
				I2C_MEMADD_SIZE_8BIT, &(timest_p->DAY), 2, Timeout);
	if(rtn_r != HAL_OK)
		return rtn_r;

	uint8_t month_r;
	if(timest_p->CENTURY)
		month_r |= BIT(7);
	month_r |= (timest_p->MONTH&B8(00011111));
	rtn_r = HAL_I2C_Mem_Write(hi2c, DS3231_BASE_ADR, DS3231_ADR_C_MONTH,
				I2C_MEMADD_SIZE_8BIT, &(month_r), 1, Timeout);
	if(rtn_r != HAL_OK)
		return rtn_r;

	rtn_r = HAL_I2C_Mem_Write(hi2c, DS3231_BASE_ADR, DS3231_ADR_YEAR,
				I2C_MEMADD_SIZE_8BIT, &(timest_p->YEAR), 1, Timeout);
	if(rtn_r != HAL_OK)
		return rtn_r;

	return HAL_OK;
}

/*
HAL_StatusTypeDef DS3231_GetTime_Partial(I2C_HandleTypeDef* hi2c, DS3231_TSECT_E section,\
		DS3231_time_t* timest_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_SetTime_Partial(I2C_HandleTypeDef* hi2c, DS3231_TSECT_E section,\
		DS3231_time_t* timest_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_WriteAlarm(I2C_HandleTypeDef* hi2c, uint8_t ch,\
		DS3231_alarm_t* alrmst_p, uint32_t Timeout);
HAL_StatusTypeDef DS3231_ReadAlarm(I2C_HandleTypeDef* hi2c, uint8_t ch,\
		DS3231_alarm_t* alrmst_p, uint32_t Timeout);
int DS3231_CheckAlarm(I2C_HandleTypeDef* hi2c, uint8_t ch, uint32_t Timeout);
*/

/**
  * @brief  Extract BCD from a packed BCD
  * @param  target buffer, 2bytes
  * @param	data in
  * @param  bits to be masked from the MSB
  * @retval Nothing
*/
void DS3231_ExtractBCD (uint8_t* dest, uint8_t input, uint8_t lead){
	*(dest+1) = input&B8(00001111);
	*dest = (input>>4)&(B8(00001111)>>lead);
	return;
}

/**
  * @brief  Build a packed BCD
  * @param  target buffer, 2bytes
  * @param	data in, MSB
  * @param	data in, LSB
  * @retval Nothing
*/
void DS3231_PackBCD (uint8_t* dest, uint8_t input_h, uint8_t input_l){
	*(dest+1) = (input_h<<4)|input_l;
	return;
}
