#include "stm32f10x.h"
#include <math.h> //при подключении этой биб-ки в линкер пишется m;
#include "Sinys_PWM_PA4_KEY.h"

int a;//глобальные переменные
int i;
char SinysStop;
unsigned short arr[256]; //количество ячеик массива счет с 1;
				//глобальный массив

//описание того что делает прерывание
void EXTI0_IRQHandler (void)
{
	//если нулевой пин модуля IDR регистра GPIOA равен 1, то
	if (GPIOA->IDR & GPIO_IDR_IDR0)
	{

		TIM3->CCER |= TIM_CCER_CC4E;//активация ШИМ выхода 4 таймера 3
		TIM6->CR1 |= TIM_CR1_CEN; //включение таймера 6
	}
	else
	{
		TIM3->CCER &= ~TIM_CCER_CC4E;//разблокируем модуль сравнения данных таймера 3
		SinysStop=1;
	}

	EXTI->PR |= EXTI_PR_PR0;/*Cброс (тригер для перезагрузки прерывания
							после срабатывания он отменяет операцию прерывания
							чтобы потом снова сработать по активации прерывания)
							флаг события прерывания. */
}
	void DMA1_Chanel3_IRQHandler (void)
	{
		if (DMA1->ISR & DMA_ISR_TCIF3)
		{
			if (SinysStop)
			{
				TIM6->CCMR1 &= ~TIM_CR1_CEN;
				SinysStop = 0;
			}
			DMA1->IFCR |= DMA_IFCR_CGIF3;
		}
	}

int main(void)
{

	for (int i=0; i<=255; i++)
		{	//урав-е для построения синусойды;
			int	value = (sin(i*2*3.141595/256)+1)*(4096/2);
			arr[i]=value;
		}
	Sinys_PA4_KEY();
	PWM_PC9_A0_TIM3();
	NVIC_EnableIRQ (EXTI0_IRQn);// Разрешение прерываний пина PA0


    while(1)
    {
    }
}
