#include "stm32f10x.h"
#include "Sinys_PWM_PA4_KEY.h"

extern unsigned short arr [256]; //глобальная переменная которая опреде-на в
							  //другом файле


void Sinys_PA4_KEY (void)
{

	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

	//PA4 analog (DAC), PA0 KEY
	GPIOA->CRL &=  0xFFF0FFF0;//записывает 0 туда где 0;
	GPIOA->CRL |=  0x00000008;

	//Подключение ЦАП к шине так-я APB1
	RCC-> APB1ENR |= RCC_APB1ENR_DACEN;

	//Включение канала ЦАП1
	DAC->CR |= DAC_CR_DMAEN1; //Разрешение ЦАПу работать с DMA;
	DAC->CR |= DAC_CR_EN1; //Включение ЦАП

// TIM6+IN.DAC+IN.DMA
//------------------------------------------------------------------


	//Включение таймера 6 на шине APB1
		RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

		TIM6->PSC = 1-1;//(24)На сколько делиться частота контроллера
								//24 000 000 /24= 1 000 000

		TIM6->ARR=4-1; //(1000)до скольки таймер считает перед сбросом
					   //1000/1 000 000 = 1кГц


		TIM6->DIER |= TIM_DIER_UDE;  // влк. запуск тригера DMA по событию

		TIM6->CR2 &= ~TIM_CR2_MMS;// обнуление
		TIM6->CR2 |= TIM_CR2_MMS_1;  // ЦАП будет запускаться по переполнению




//Настройка DMA1_Chanel3
//-----------------------------------------------------------------------------------------------------
		//Канал смотрится в RM поиск по "requests for each channel"
	    RCC->AHBENR |= RCC_AHBENR_DMA1EN;       // подаю такты на DMA1
	    DMA1_Channel3->CCR |= DMA_CCR3_MSIZE_0; // будем передавать 16 бит данных
	    DMA1_Channel3->CCR |= DMA_CCR3_PSIZE_0; // 16 бит для общения с перефе-м устрой-м
	    DMA1_Channel3->CCR |= DMA_CCR3_MINC;    // увеличение адреса памяти на DMA_MSIZE;
	    DMA1_Channel3->CCR |= DMA_CCR3_CIRC;    // циклический режим передачи
	    DMA1_Channel3->CCR |= DMA_CCR3_DIR;     // откуда идет чтение (с памяти)
	    DMA1_Channel3->CCR |= DMA_CCR3_TEIE; // Прерывание по завершению цикла

	    DMA1_Channel3->CNDTR = 256; // для того чтобы работало
	    DMA1_Channel3->CPAR = (uint32_t) &DAC->DHR12R1; //Сылка на адрес откуда брать значение
	    DMA1_Channel3->CMAR = (uint32_t) &arr[0]; //откуда начинать чтение

	    DMA1_Channel3->CCR |= DMA_CCR3_EN; // вкл. ПДП
	    NVIC_SetPriority(DMA1_Channel3_IRQn, 1); // установка приоритета прерывания DMA
	    NVIC_EnableIRQ(DMA1_Channel3_IRQn); // ВКЛЮЧ прерываний для DMA
}
