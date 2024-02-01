#include "stm32f10x.h"
#include "Sinys_PWM_PA4_KEY.h"


void PWM_PC9_A0_TIM3 (void)
	{
		//��������� �������������� ������� �����������
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

		//��������� ����� � � � ���� APB2
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

		// ��������� PC9 � PC8 � PA0
		//��������  ������� PC9, PC8 50MGz

		//���������-� ���-� ����-�
		GPIOC->CRH |= GPIO_CRH_MODE9_1  |  GPIO_CRH_MODE9_0 | GPIO_CRH_CNF9_1;

		GPIOC->CRH &= ~GPIO_CRH_CNF9_0;
		GPIOA->CRL &= ~GPIO_CRL_MODE0_0 & ~GPIO_CRL_MODE0_1 & ~GPIO_CRL_CNF0_0;
		GPIOA->CRL |= GPIO_CRL_CNF0_1;

		//GPIOC->ODR |= GPIO_ODR_ODR9;//���������� 9-� ��� � 1. ��������� 9-� �����
		EXTI->RTSR |= EXTI_RTSR_TR0;// ���������� �� ����������� ������ �� ����� 0
		EXTI->FTSR |= EXTI_RTSR_TR0;//���������� �� ���������� ������ �� ����� 0
		EXTI->IMR |= EXTI_IMR_MR0; //��������� ���������� ��� ����� 0
		AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;//������ ������ ���������� (������ PA0)

	//��������� �������������� ������� ������� 3 ��� ����� PC9
	AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP;

	//��������� ������� 2 �� ���� APB1
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3->PSC = 24;//�� ������� �������� ������� �����������
							//24 000 000 /24= 1 000 000

	TIM3->ARR=10; //�� ������� ������ ������� ����� �������
				 // 10/1 000 000 = 100���
	/*����� ��������� � �������, ���� 1/100���=0.00001 � (������)
	� �������� �� ��������� CCR4/ARR=0.1 (0.1)*0.00001 ��� ����� ��������� */

	TIM3->CCR4 = 5; //(���� �����-� ���) �� ������� ������ ������� ����� �������
							//���� �����-�: (�) = ��CR4/ARR *100% = 5/10*100=50%

	//TIM3->CCER |=TIM_CCER_CC4E;//������������ ������ ��������� ������ ������� 3

	TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;//����� ���1 ������� 3

	TIM3->CR1 |= TIM_CR1_CEN; //��������� ������� 3
	}
