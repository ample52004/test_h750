
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_TIM_H__
#define __HAL_TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tim.h"


typedef struct
{
	uint8_t TIM_Status;							// TIM״̬
#define TIM_STATUS_IDLE		0x00		// TIM״̬����
#define TIM_STATUS_BUSY		0x01		// TIM״̬æ
}TIM_STATUS_TypeDef;


#define TIM_HAL_STRUCT								htim2
#define TIM_PERIPHERAL								TIM2
#define TIM_PERIPHERAL_IRQN						TIM_PERIPHERAL_IRQn
#define TIM_PERIPHERAL_RCC_ENABLE()   __HAL_RCC_TIM2_CLK_ENABLE()
#define TIM_PSC_REGISTER							TIM_PERIPHERAL->PSC		// TIMԤ��Ƶ�Ĵ���
#define TIM_ARR_REGISTER							TIM_PERIPHERAL->ARR		// TIM��װ�ؼĴ���
#define TIM_CNT_REGISTER							TIM_PERIPHERAL->CNT		// TIM������


void TIM_HAL_Init(uint16_t Prescaler, uint16_t Period);

extern TIM_STATUS_TypeDef TIM_STATUS_Struct;


#ifdef __cplusplus
}
#endif

#endif


