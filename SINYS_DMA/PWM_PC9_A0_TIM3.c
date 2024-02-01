#include "stm32f10x.h"
#include "Sinys_PWM_PA4_KEY.h"


void PWM_PC9_A0_TIM3 (void)
	{
		//Включение альтернативных функций контроллера
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

		//Включение порта С и А шины APB2
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

		// Настройка PC9 и PC8 и PA0
		//скорость  сигнала PC9, PC8 50MGz

		//Альтернав-й реж-м рабо-ы
		GPIOC->CRH |= GPIO_CRH_MODE9_1  |  GPIO_CRH_MODE9_0 | GPIO_CRH_CNF9_1;

		GPIOC->CRH &= ~GPIO_CRH_CNF9_0;
		GPIOA->CRL &= ~GPIO_CRL_MODE0_0 & ~GPIO_CRL_MODE0_1 & ~GPIO_CRL_CNF0_0;
		GPIOA->CRL |= GPIO_CRL_CNF0_1;

		//GPIOC->ODR |= GPIO_ODR_ODR9;//установить 9-й бит в 1. Включение 9-й ножки
		EXTI->RTSR |= EXTI_RTSR_TR0;// Прерывания по восзодящему фронту по ножке 0
		EXTI->FTSR |= EXTI_RTSR_TR0;//Прерывания по убывающему фронту по ножке 0
		EXTI->IMR |= EXTI_IMR_MR0; //Включение прерываний для линии 0
		AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;//Откуда ловить прерывания (кнопка PA0)

	//Включение альтернативной функции таймера 3 для ножки PC9
	AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP;

	//Включение таймера 2 на шине APB1
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3->PSC = 24;//На сколько делиться частота контроллера
							//24 000 000 /24= 1 000 000

	TIM3->ARR=10; //до скольки таймер считает перед сбросом
				 // 10/1 000 000 = 100кГц
	/*чтобы перевести в секунды, надо 1/100кГц=0.00001 с (период)
	и умножить на отношение CCR4/ARR=0.1 (0.1)*0.00001 сек горит светодиод */

	TIM3->CCR4 = 5; //(коэф запол-я ШИМ) до скольки таймер считает перед сбросом
							//Коэф запол-я: (К) = ССCR4/ARR *100% = 5/10*100=50%

	//TIM3->CCER |=TIM_CCER_CC4E;//разблокируем модуль сравнения данных таймера 3

	TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;//режим ШИМ1 таймера 3

	TIM3->CR1 |= TIM_CR1_CEN; //вкдючение таймера 3
	}
