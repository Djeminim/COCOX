//Включение ШИМ по нажатию кнопки STM32F100RB

#include "stm32f10x.h"
#include "GPIOC_8_9_GPIOA_0_PRERIV.с"
#include "TIM3_AFIO.с"

//описание того что делает прерывание
void EXTI0_IRQHandler (void)
{
	//если нулевой пин модуля IDR регистра GPIOC равен 1, то
	if (GPIOA->IDR & GPIO_IDR_IDR0)
	{
		TIM3->CCER |= TIM_CCER_CC4E;//активация ШИМ выхода 4 таймера 3
	}
	else
	{
		TIM3->CCER &= ~TIM_CCER_CC4E;//разблокируем модуль сравнения данных таймера 3
	}

	EXTI->PR |= EXTI_PR_PR0;/*Cброс (тригер для перезагрузки прерывания
							после срабатывания он отменяет операцию прерывания
							чтобы потом снова сработать по активации						прерывания)
							флаг события прерывания. */
}

void GPIOInit1 (void)
{
	//Включение альтернативных функций контроллера
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

	//Включение порта С и А шины APB2
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	// Настройка PC9 и PC8 и PA0
	//частота приема сигнала PC9, PC8 50MGz
	GPIOC->CRH |= GPIO_CRH_MODE9_1  | GPIO_CRH_MODE8_1 | GPIO_CRH_MODE9_0 | GPIO_CRH_MODE8_0;
	GPIOC->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_CNF8_1;//Альтернав-й реж-м рабо-ы
	GPIOC->CRH &= ~GPIO_CRH_CNF9_0 & ~GPIO_CRH_CNF8_0;
	GPIOA->CRL &= ~GPIO_CRL_MODE0_0 & ~GPIO_CRL_MODE0_1 & ~GPIO_CRL_CNF0_0;
	GPIOA->CRL |= GPIO_CRL_CNF0_1;

	//GPIOC->ODR |= GPIO_ODR_ODR9;//установить 9-й бит в 1. Включение 9-й ножки
	EXTI->RTSR |= EXTI_RTSR_TR0;// Прерывания по восзодящему фронту по ножке 0
	EXTI->FTSR |= EXTI_RTSR_TR0;//Прерывания по убывающему фронту по ножке 0
	EXTI->IMR |= EXTI_IMR_MR0; //Включение прерываний для линии 0
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;//Откуда ловить прерывания (кнопка PA0)
}

void TIM3Init (void)
{
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


int main(void)
{
	GPIOInit1();
	TIM3Init();

NVIC_EnableIRQ (EXTI0_IRQn);// Разрешение прерываний пина PA0
while(1)
{
}
}
