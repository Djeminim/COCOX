#include "stm32f10x.h"
#include "Sinys_PWM_PA4_KEY.h"

extern unsigned short arr [256]; //���������� ���������� ������� ������-�� �
							  //������ �����


void Sinys_PA4_KEY (void)
{

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	//PA4 analog (DAC), PA0 KEY
	GPIOA->CRL &=  0xFFF0FFF0;//���������� 0 ���� ��� 0;
	GPIOA->CRL |=  0x00000008;

	//����������� ��� � ���� ���-� APB1
	RCC-> APB1ENR |= RCC_APB1ENR_DACEN;

	//��������� ������ ���1
	DAC->CR |= DAC_CR_DMAEN1; //���������� ���� �������� � DMA;
	DAC->CR |= DAC_CR_EN1; //��������� ���

// TIM6+IN.DAC+IN.DMA
//------------------------------------------------------------------


	//��������� ������� 6 �� ���� APB1
		RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

		TIM6->PSC = 1-1;//(24)�� ������� �������� ������� �����������
								//24 000 000 /24= 1 000 000

		TIM6->ARR=4-1; //(1000)�� ������� ������ ������� ����� �������
					   //1000/1 000 000 = 1���


		TIM6->DIER |= TIM_DIER_UDE;  // ���. ������ ������� DMA �� �������

		TIM6->CR2 &= ~TIM_CR2_MMS;// ���������
		TIM6->CR2 |= TIM_CR2_MMS_1;  // ��� ����� ����������� �� ������������




//��������� DMA1_Chanel3
//-----------------------------------------------------------------------------------------------------
		//����� ��������� � RM ����� �� "requests for each channel"
	    RCC->AHBENR |= RCC_AHBENR_DMA1EN;       // ����� ����� �� DMA1
	    DMA1_Channel3->CCR |= DMA_CCR3_MSIZE_0; // ����� ���������� 16 ��� ������
	    DMA1_Channel3->CCR |= DMA_CCR3_PSIZE_0; // 16 ��� ��� ������� � ������-� ������-�
	    DMA1_Channel3->CCR |= DMA_CCR3_MINC;    // ���������� ������ ������ �� DMA_MSIZE;
	    DMA1_Channel3->CCR |= DMA_CCR3_CIRC;    // ����������� ����� ��������
	    DMA1_Channel3->CCR |= DMA_CCR3_DIR;     // ������ ���� ������ (� ������)
	    DMA1_Channel3->CCR |= DMA_CCR3_TEIE; // ���������� �� ���������� �����

	    DMA1_Channel3->CNDTR = 256; // ��� ���� ����� ��������
	    DMA1_Channel3->CPAR = (uint32_t) &DAC->DHR12R1; //����� �� ����� ������ ����� ��������
	    DMA1_Channel3->CMAR = (uint32_t) &arr[0]; //������ �������� ������

	    DMA1_Channel3->CCR |= DMA_CCR3_EN; // ���. ���
	    NVIC_SetPriority(DMA1_Channel3_IRQn, 1); // ��������� ���������� ���������� DMA
	    NVIC_EnableIRQ(DMA1_Channel3_IRQn); // ����� ���������� ��� DMA
}
