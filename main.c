#include "stm32f0xx.h"          
#include "stm32f0xx_gpio.h"     // Pins Interface
#include "stm32f0xx_rcc.h"      // Enable peripheral clock
#include "stm32f0xx_usart.h"      // 

void led_toggle(void);
GPIO_TypeDef* GetGPIOPort(uint16_t port)
{
	switch((port & 0xF0)>>4)
	{
	case 0xA: return GPIOA;
	case 0xB: return GPIOB;
	case 0xC: return GPIOC;
	case 0xD: return GPIOD;
	//case 0xE: return GPIOE;
	case 0xF: return GPIOF;

	}
}
uint32_t GetGPIOPpin(uint16_t pin)
{
	switch(pin&0x0F)
	{
	case 0: return GPIO_Pin_0;
	case 1: return GPIO_Pin_1;
	case 2: return GPIO_Pin_2;
	case 3: return GPIO_Pin_3;
	case 4: return GPIO_Pin_4;
	case 5: return GPIO_Pin_5;
	case 6: return GPIO_Pin_6;
	case 7: return GPIO_Pin_7;
	case 8: return GPIO_Pin_8;
	case 9: return GPIO_Pin_9;
	case 10: return GPIO_Pin_10;
	case 11: return GPIO_Pin_11;
	case 12: return GPIO_Pin_12;
	case 13: return GPIO_Pin_13;
	case 14: return GPIO_Pin_14;
	case 15: return GPIO_Pin_15;
	}
}


/// Init USART1 port
void InitUSART1()
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_Cmd(USART1,ENABLE);

	USART_HalfDuplexCmd(USART1, ENABLE);
	
	
    /* Enable RXNE interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable USART1 global interrupt */
    NVIC_EnableIRQ(USART1_IRQn);
}

void InitGPIOs()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 |  GPIO_Pin_4 |  GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	uint16_t portPin = 0xA1;
	GPIO_TypeDef* gpio_port = GetGPIOPort(portPin);
	uint32_t gpio_pin = GetGPIOPpin(portPin);
	GPIO_WriteBit(gpio_port,gpio_pin,1);
	
	portPin = 0xA3;
	gpio_pin = GetGPIOPpin(portPin);
	gpio_port = GetGPIOPort(portPin);
	GPIO_WriteBit(gpio_port,gpio_pin,0);
	
	portPin = 0xA5;
	gpio_pin = GetGPIOPpin(portPin);
	gpio_port = GetGPIOPort(portPin);
	GPIO_WriteBit(gpio_port,gpio_pin,0);
	
	portPin = 0xA4;
	gpio_pin = GetGPIOPpin(portPin);
	gpio_port = GetGPIOPort(portPin);
	GPIO_WriteBit(gpio_port,gpio_pin,0);
	
	portPin = 0xA4;
	gpio_pin = GetGPIOPpin(portPin);
	gpio_port = GetGPIOPort(portPin);
	GPIO_WriteBit(gpio_port,gpio_pin,1);
	
}

void USART1_Send(uint16_t byte)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, byte);
}

void SetupTimer(uint32_t delay)
{
	RCC-> APB1ENR |= RCC_APB1ENR_TIM2EN; // clock file on TIM2
			TIM2-> PSC = 24000-1; // adjust the divisor for the formation of millisecond
			TIM2-> CR1 |= TIM_CR1_CEN; // starts the timer
			TIM2->ARR=delay;//load the value of set backs
			TIM2->CNT=0;//??zero out counter
			TIM2->SR&=~TIM_SR_UIF;//reset the flag
			TIM2->SR &= !TIM_SR_UIF;
			TIM2->CR1=TIM_CR1_CEN;//start the timer
}

int IsTimerExpired()
{
	return (TIM2-> SR & TIM_SR_UIF) == 0;
}

//Wait for USAART1 RX data and return value.
uint16_t USART1_Read()
{
	uint16_t value;
	SetupTimer(5);
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET )
	{
		if(!IsTimerExpired())
				return 0xFFFF;
	}
	value=USART_ReceiveData(USART1);
	return value;

}

int main(void) 
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
		
		InitGPIOs();
		InitUSART1();
		int i =0;
		char data[] = "Aganya";
		uint16_t recive = 0;
		uint16_t send = (uint16_t)"Aganya";
		
		
		while(1)
		{
			for (i=0 ; i< sizeof(data);i++)
			{
				if (data[i] != 0x00)
					USART1_Send(data[i]);
			}
			
//			 recive = USART1_Read();
//			if (recive!=0 && recive!= 0xFFFF)
//			{
//				USART1_Send(recive);
//				recive = 0;
//			}
			
		
			
		}
	}
	
	void USART1_IRQHandler(void)
{
    /* RXNE handler */
//    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
//    {
        /* If received 't', toggle LED and transmit 'T' */
        if((char)USART1_Read() == 't')
        {
            led_toggle();
            USART_SendData(USART1, 'T');
            /* Wait until Tx data register is empty, not really 
             * required for this example but put in here anyway.
             */
            /*
            while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
            {
            }*/
        }
    //}
     
    /* ------------------------------------------------------------ */
    /* Other USART1 interrupts handler can go here ...             */
}  


void led_toggle(void)
{
    /* Read LED output (GPIOA PIN8) status */
    uint8_t led_bit = GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8);
     
    /* If LED output set, clear it */
    if(led_bit == (uint8_t)Bit_SET)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_8);
    }
    /* If LED output clear, set it */
    else
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_8);
    }
}
		
//void SetLedState(int state)
//	{
//		GPIO_WriteBit(GetGPIOPort(StatusLed),GetGPIOPpin(StatusLed),state);
//	}
//			
	
	

	


