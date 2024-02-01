#include "stm32f10x.h"
#include <math.h> //��� ����������� ���� ���-�� � ������ ������� m;
#include "Sinys_PWM_PA4_KEY.h"
//#include "PWM_PC9_TIM3.h"

int a;//���������� ����������
int i;
char SinysStop;
unsigned short arr[256]; //���������� ����� ������� ���� � 1;
				//���������� ������

//�������� ���� ��� ������ ����������
void EXTI0_IRQHandler (void)
{
	//���� ������� ��� ������ IDR �������� GPIOC ����� 1, ��
	if (GPIOA->IDR & GPIO_IDR_IDR0)
	{

		TIM3->CCER |= TIM_CCER_CC4E;//��������� ��� ������ 4 ������� 3
		TIM6->CR1 |= TIM_CR1_CEN; //��������� ������� 6
	}
	else
	{
		TIM3->CCER &= ~TIM_CCER_CC4E;//������������ ������ ��������� ������ ������� 3
		SinysStop=1;
	}

	EXTI->PR |= EXTI_PR_PR0;/*C���� (������ ��� ������������ ����������
							����� ������������ �� �������� �������� ����������
							����� ����� ����� ��������� �� ��������� ����������)
							���� ������� ����������. */
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
		{	//����-� ��� ���������� ���������;
			int	value = (sin(i*2*3.141595/256)+1)*(4096/2);
			arr[i]=value;
		}
	Sinys_PA4_KEY();
	PWM_PC9_A0_TIM3();
	NVIC_EnableIRQ (EXTI0_IRQn);// ���������� ���������� ���� PA0


    while(1)
    {
    }
}
