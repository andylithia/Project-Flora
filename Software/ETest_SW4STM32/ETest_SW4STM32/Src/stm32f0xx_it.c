/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"

/* USER CODE BEGIN 0 */
#include "BCG.h"
#include "gvar.h"
#include "flora_BSP.h"
#include "../Menuitems/menu_l0_intro.h"
#include "BCDUtil.h"
#include "string.h"

// 0.A, 1.B, 2.Key
uint8_t quad_in[3];
uint8_t quad_flag[3];
uint8_t quad_fw_rvn;
uint8_t quad_rotate;
uint8_t quad_kdn;
union { uint32_t HL; struct{uint16_t H; uint16_t L;}; } quad_kdn_length;
uint8_t cnt;

extern I2C_HandleTypeDef hi2c1;
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
	NVIC_DisableIRQ(TIM16_IRQn);
	memcpy(g_textBuffer, "!HFT", 4);
	IV9_send(g_textBuffer,4,B8(00001111));
	IV9_FastPWMSet(&htim2, B8(11111111), 239);
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles TIM2 global interrupt.
*/
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
* @brief This function handles TIM16 global interrupt.
*/
void TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM16_IRQn 0 */

  /* USER CODE END TIM16_IRQn 0 */
  HAL_TIM_IRQHandler(&htim16);
  /* USER CODE BEGIN TIM16_IRQn 1 */

  /* USER CODE END TIM16_IRQn 1 */
}

/**
* @brief This function handles TIM17 global interrupt.
*/
void TIM17_IRQHandler(void)
{
  /* USER CODE BEGIN TIM17_IRQn 0 */

  /* USER CODE END TIM17_IRQn 0 */
  HAL_TIM_IRQHandler(&htim17);
  /* USER CODE BEGIN TIM17_IRQn 1 */

  /* USER CODE END TIM17_IRQn 1 */
}

/* USER CODE BEGIN 1 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){

	/* 500Hz Interrupt */
	if(htim==&htim16){
		quad_in[0] = !HAL_GPIO_ReadPin(QUAD_A_GPIO_Port,QUAD_A_Pin);
		quad_in[1] = !HAL_GPIO_ReadPin(QUAD_B_GPIO_Port,QUAD_B_Pin);
		quad_in[2] = !HAL_GPIO_ReadPin(QUAD_KEY_GPIO_Port,QUAD_KEY_Pin);

		if(quad_in[0]){
			if(quad_in[1]){
				quad_flag[0] = 0;
				quad_flag[1] = 0;
			}
		} else if (!quad_flag[0]) {
			quad_flag[0] = 1;
			if(quad_flag[1]){
				quad_fw_rvn = 0;
				quad_rotate = 1;
			}
		}

		if(quad_in[1]){
			if(quad_in[0]){
				quad_flag[0] = 0;
				quad_flag[1] = 0;
			}
		} else if (!quad_flag[1]) {
			quad_flag[1] = 1;
			if(quad_flag[0]){
				quad_fw_rvn = 1;
				quad_rotate = 1;
			}
		}

		if(quad_in[2]){
			if(!quad_flag[2]){
				quad_flag[2] = 1;
				if(quad_kdn_length.H<KDN_CANCEL_PERIOD)
					quad_kdn = 1;
				else
					quad_kdn_length.HL=0;
			}
		} else {
			quad_flag[2]= 0;
			quad_kdn_length.HL++;
		}

		if(quad_rotate||quad_kdn){
			quad_fw_rvn |= (quad_kdn << QINF_BIT_KDN);
			quad_fw_rvn |= (quad_rotate << QINF_BIT_ROT);
			if(quad_kdn_length.H>=KDN_LONG_PRESS_PERIOD)
				quad_fw_rvn |= BIT(QINF_BIT_LONG_PRESS);

			quad_rotate = 0;
			quad_kdn = 0;
			quad_kdn_length.HL = 0;

			MFA_CALL(g_currentMenu,EVENT_QUAD,&quad_fw_rvn);
		}

		MFA_CALL(g_currentMenu, EVENT_SSEC, 0);

		if(g_sysCfg&CONF_TIME_FETCH)
			DS3231_GetTime(&hi2c1, &g_time_reg, 0xFF);

		if(!(g_sysCfg&CONF_DISP_INHIBIT)||(g_sysIssue&ISSUE_DISP)){
			if(g_sysIssue&ISSUE_DISP)
				g_sysIssue &= ~ISSUE_DISP;
			IV9_send(g_textBuffer,4,g_dpBuffer);
		}


	} /* htim==&htim16 */
} /* HAL_TIM_PeriodElapsedCallback */
/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
