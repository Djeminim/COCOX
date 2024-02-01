#include "stm32f10x.h"

unsigned char Ekran[16] ;
unsigned int i;
unsigned int Time;
unsigned int Taktov;
unsigned int command;
unsigned	int data;
uint32_t count_tic = 0; 
unsigned int sekynd = 0; //64 áèòà /8 áàéò
unsigned char Sek=0x30;
unsigned char Sek2=0x30;
unsigned char Min=0x30;
unsigned char Min2=0x30;
unsigned char Hours=0x30;
unsigned char Hours2=0x30;
unsigned char Vigryzka;

/*Cчетчик (измерить время выполнения какого-либо участка программы или функции)
void DWT_Init (void)
{
	 SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;// разрешаем использовать DWT
	 DWT_CONTROL|= DWT_CTRL_CYCCNTENA_Msk; // включаем счeтчик
	 DWT_CYCCNT = 0;// обнуляем счeтчик

	  // здесь кусок измеряемого участка программы

	 count_tic = DWT_CYCCNT; // кол-во тактов
}
*/



unsigned char Send (unsigned char bait)
{
	unsigned short Spi2TimeOut = 0;
	while ((!(SPI2->SR & SPI_SR_TXE)) && (Spi2TimeOut < 1000)) {Spi2TimeOut++;}
	Spi2TimeOut = 0;
	SPI2->DR = bait; //Загрузка в др
	while ((!(SPI2->SR & SPI_SR_RXNE)) && (Spi2TimeOut < 1000)) {Spi2TimeOut++;}
	return SPI2->DR; //Выгрузка из др (Обязательно нужно делать, для перезарядки)
}

void Delay_mc (unsigned char Time)
{
	Taktov = Time * 2178; //2178 - (столько тактов в 1mc)
	for (Taktov=0; Taktov<Time; Taktov++);
}



void Data (unsigned char D) //данные для контроллера
{
	GPIOB->ODR &= ~GPIO_ODR_ODR12;

	for (int k=0; k<1;k++); //Порой нужно делать задержку
						   //Чтобы контроллер диплея успел
						   //обработать данные

	Send (0x5F); //код который определяет data это или comand
	Send ((D)&0x0F);
	Send (((D&0xF0)>>4)&0x0F);

	for (int k=0; k<1;k++);
	GPIOB->ODR |= GPIO_ODR_ODR12;
	for (int k=0; k<1;k++);
}

void Command (unsigned char C) //Команда для контроллера
{
	GPIOB->ODR &= ~GPIO_ODR_ODR12;
	for (int k=0; k<1;k++);

	Send (0x1F);
	Send ((C)&0x0F);
	Send (((C&0xF0)>>4)&0x0F);

	for (int k=0; k<1;k++);
	GPIOB->ODR &= ~GPIO_ODR_ODR12;
	for (int k=0; k<1;k++);
}



void TIME (void )
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	TIM6->PSC = 24000 - 1; //24 000 000/24 000 = 1 000 Гц
	TIM6->ARR = 1000 - 1; // 1 000/1000 = 1 сек
	TIM6->DIER |= TIM_DIER_UIE; //разрешение прерываний таймера 6
	TIM6->CR1 |= TIM_CR1_ARPE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn); //включаем функцию прерыванию по переполнению ARR
	TIM6->CR1 |= TIM_CR1_CEN;  // Запускаем таймер
}

void SPI2_Init (void)
{
	//Включение тактирование SPI2, порта А,B
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //включение альтернативных функций
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

	GPIOA->CRH &= 0xFFFFFFF0; //обнуляем PA8 /RESET
	GPIOA->CRH |= 0x00000003; //PA8 50MHz General output push-pull;

	GPIOC->CRL &= 0xF0FFFFFF; //обнуляем PA8 /RESET
	GPIOC->CRL |= 0x03000000; //PC6 50MHz General output push-pull;


	 // Конфигурируем выводы SPI2 (смотреть в как настроены пины в Prod-t Speci-n 7.1 SPI). ->
	 // PB12, SPI2_NSS: Если мы управляем тогда General purpose output push-pull
	 // PB13, SPI2_SCK:(Master) Alternate function push-pull
	 // PB14, SPI2_MISO: input, pull up/down
	 // PB15, SPI2_MOSI: (Mast	er) Alternate function push-pull
	 // PA8 , RESET
	GPIOB->CRH &= 0x0000FFFF;  //где 0 там очищается
	GPIOB->CRH |= 0xB4B30000;

	GPIOB->ODR |= GPIO_ODR_ODR12; //Обзательно поднять в 1 линию CS (для мале-го дисп-я)


	//Альтернативная настройка SPI
	SPI2->CR1 &= ~SPI_CR1_DFF;  //ПЕРЕДАЧА 8 БИТ

	SPI2->CR1 |= SPI_CR1_LSBFIRST;//MSB first

	SPI2->CR1 |= SPI_CR1_SSM;  //Программное управление ведомым устрой-м SS

	SPI2->CR1 |= SPI_CR1_SSI;  //SS в высоком состоянии

	SPI2->CR1 &= ~SPI_CR1_BR;  //Скорость передачи: F_PCLK/2

	SPI2->CR1 |= SPI_CR1_MSTR;  //Режим Master (ведущий)

	SPI2->CR1 &= ~SPI_CR1_CPOL & ~SPI_CR1_CPHA;  //Режим работы SPI: 0

	SPI2->CR2 |= SPI_CR2_TXDMAEN; //Обращение к DMA если упешно передали данные по SPI и буфер TX готов к передачи

	SPI2->CR2 |= SPI_CR2_RXDMAEN; //Обращение к DMA если упешно приняли данные по SPI и буфер TX готов к приему

	SPI2->CR1 |= SPI_CR1_SPE;  //Включаем SPI
}

void DMA_Init (void)
{
	//Настраиваем 3-й канал DMA
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	//DMA1_Channel3->CCR |= DMA_CCR3_MSIZE_0 ; //не настраиваем по умолчанию передача 8 бит
	//DMA1_Channel3->CCR |= DMA_CCR3_PSIZE_0 ; //не настраиваем по умолчанию передача 8 бит
	//DMA1_Channel5->CCR |= DMA_CCR5_CIRC; //включ циклического режима
	DMA1_Channel5->CCR |= DMA_CCR5_DIR;//Чтение с памяти
	DMA1_Channel5->CCR |= DMA_CCR5_TCIE;//Прерывание при завершении цикла
	DMA1_Channel5->CCR |= DMA_CCR5_MINC;//Включить Инкремент адреса памяти на величену установленную вCRR_MSIZE

	DMA1_Channel5->CMAR |= (uint32_t) &Ekran[0]; /* Адрес памяти. uint32_t обязательно
	писать, т.к. все адреса в stm32 32-х битные и если не написать, он не распознает адрес.*/

	DMA1_Channel5->CPAR |= (uint32_t) &SPI2->DR; //Адрес переферии
	NVIC_EnableIRQ(DMA1_Channel5_IRQn);

//---------------------------------------------------------------------------

	DMA1_Channel4->CCR |= DMA_CCR4_CIRC;
	DMA1_Channel4->CCR &= ~DMA_CCR4_DIR; //чтение с переферии
	DMA1_Channel4->CCR |= DMA_CCR4_TCIE;

	DMA1_Channel4->CPAR |= (uint32_t) &SPI2->DR;
	DMA1_Channel4->CMAR |= (uint32_t) &Vigryzka;

	NVIC_EnableIRQ(DMA1_Channel4_IRQn);

}

void TIM6_DAC_IRQHandler(void)
{
	sekynd++;
	Command(0x80);
	Command(0x80);

	Ekran[0]=(0x5F);
	Ekran[1]=(((((((sekynd/3600)%24)/10))+0x30))&0x0F);//часы больше 10
	Ekran[2]=((((((((sekynd/3600)%24)/10))+0x30))>>4)&0x0F);

	Ekran[3]=(((((((sekynd/3600)%24)%10))+0x30))&0x0F);//часы меньше 10
	Ekran[4]=((((((((sekynd/3600)%24)%10))+0x30))>>4)&0x0F);

	Ekran[5]= (':'&0x0F);
	Ekran[6]= ((':'>>4)&0x0F);

	Ekran[7]=(((((((sekynd/60)%60)/10))+0x30))&0x0F);//минуты больше 10
	Ekran[8]=((((((((sekynd/60)%60)/10))+0x30))>>4)&0x0F);//минуты больше 10

	Ekran[9]=(((((((sekynd/60)%60)%10))+0x30))&0x0F);//минуты меньше 10
	Ekran[10]=((((((((sekynd/60)%60)%10))+0x30))>>4)&0x0F);//минуты меньше 10

	Ekran[11]= (':'&0x0F);
	Ekran[12]= ((':'>>4)&0x0F);

	Ekran[13]=((((((sekynd%60)/10))+0x30))&0x0F); //секунды больше 10
	Ekran[14]=(((((((sekynd%60)/10))+0x30))>>4)&0x0F); //секунды больше 10

	Ekran[15]=((((((sekynd%60)%10))+0x30))&0x0F); //секунды меньше 10
	Ekran[16]=(((((((sekynd%60)%10))+0x30))>>4)&0x0F); //секунды меньше 10

	GPIOB->ODR &= ~GPIO_ODR_ODR12; //включение CR у SPI

	DMA1_Channel4->CNDTR |= 17;//кол-во отправок DMA (Лучше ставить перед DMA_EN)
	DMA1_Channel5->CNDTR |= 17;//кол-во отправок DMA (Лучше ставить перед DMA_EN)
	DMA1_Channel4->CCR |= DMA_CCR4_EN;
	DMA1_Channel5->CCR |= DMA_CCR5_EN;

	TIM6->SR &= ~TIM_SR_UIF;//Перезарядка таймера

}

void DMA1_Channel5_IRQHandler (void)
{
	DMA1_Channel5->CCR &= ~DMA_CCR5_EN;

	while (!(SPI2->SR & SPI_SR_TXE));
	while (SPI2->SR & SPI_SR_BSY);

	GPIOB->ODR |= GPIO_ODR_ODR12; //выключение CR у SPI по завершению передачи DMA
	DMA1->IFCR |=DMA_IFCR_CGIF5; //Перезарядка для прерываний DMA(отчистка модуля прерываний TEIE)
}

void DMA1_Channel4_IRQHandler (void)
{
	DMA1_Channel4->CCR &= ~DMA_CCR4_EN;

	DMA1->IFCR |=DMA_IFCR_CGIF4;
}

int main(void)
{
	SPI2_Init();

	Delay_mc(1); //1 mc 23993(тактов) (2178 тактов за 1);

	GPIOC->ODR |= GPIO_ODR_ODR6; //PA8=1 (для работы маленького дисплея)

	Delay_mc(1);
	Command (0x2A);
	Command (0x71);
	Data (0x00);
	Command (0x28);
	Command (0x08);
	Command (0x2A);
	Command (0x79);
	Command (0xD5);
	Command (0x70);
	Command (0x78);
	Command (0x08);
	Command (0x06);
	Command (0x72);
	Data (0x00);
	Command (0x2A);
	Command (0x79);
	Command (0xDA);
	Command (0x00);
	Command (0xDC);
	Command (0x00);
	Command (0x81);
	Command (0x7F);
	Command (0xD9);
	Command (0xF1);
	Command (0xDB);
	Command (0x40);
	Command (0x78);
	Command (0x28);
	Command (0x01);
	Command (0x80);
	Command (0x0C);
	Delay_mc(100); //100mc (2 400 000 тактов)

	DMA_Init();
	TIME();
	while(1)
    {
    }
}
