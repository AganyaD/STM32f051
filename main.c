#include "stm32f0xx.h"          
#include "stm32f0xx_gpio.h"     // Pins Interface
#include "stm32f0xx_rcc.h"      // Enable peripheral clock
#include "stm32f0xx_usart.h"      // 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>	
#include <stdio.h>


#define BuffSize  300
#define True			1
#define False			0
char outBuff[BuffSize];
char inBuff[BuffSize];
int inBuffIndex = 0; 
int mode = 0;
int Uart2Budrate = 57600;
char calcLogic[2] = "00";
char messageData[8] = "";
char messageID[3]="";
int breakeValue = 0;



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

void InitUSART2()
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = Uart2Budrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2,ENABLE);

	USART_HalfDuplexCmd(USART2, ENABLE);
	
	
    /* Enable RXNE interrupt */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    /* Enable USART1 global interrupt */
    NVIC_EnableIRQ(USART2_IRQn);
}

void InitGPIOs()
{
	GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |  GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  //0x48000000 GPIOA
	//GPIO_Pin_1 0002
	uint16_t portPin = 0xA1;
	GPIO_TypeDef* gpio_port = GetGPIOPort(portPin);
	uint32_t gpio_pin = GetGPIOPpin(portPin);
	GPIO_WriteBit(gpio_port,gpio_pin,1);
	
//	//0x48000000 GPIOA
//	//GPIO_Pin_1 0002
//	portPin = 0xA3;
//	gpio_pin = GetGPIOPpin(portPin);
//	gpio_port = GetGPIOPort(portPin);
//	GPIO_WriteBit(gpio_port,gpio_pin,0);
//	
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

void USART2_Send(uint16_t byte)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, byte);
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

uint16_t USART2_Read()
{
	uint16_t value;
	SetupTimer(5);
	while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET )
	{
		if(!IsTimerExpired())
				return 0xFFFF;
	}
	value=USART_ReceiveData(USART2);
	return value;

}

void SendData(char* sendPack ,USART_TypeDef* uart)
{
	int i=0;
	char outBuf[100] = "";
	strcpy(outBuf,sendPack);
	int size = sizeof(outBuf);
	//USART1_Send(data);
	for (i=0 ; i< size;i++)
			{
				
				if (outBuf[i] != 0x00)
				{
					if (uart == USART1)
						USART1_Send(outBuf[i]);
					else
						if (uart == USART2)
						USART2_Send(outBuf[i]);
				}
				else
					break;
			}
}

/////////////////////////////////////MAIN//////////////////////////////////
int main(void) 
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
		
		InitGPIOs();
		InitUSART1();
		InitUSART2();
//		GPIO_Init(GPIOC,
//		GPIO_DeInit(GPIOC);
//		GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);
		GPIO_SetBits(GPIOC,GPIO_Pin_8);
//		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
		SendData("Aganya - start Program Uart 2 ",USART2);
		SendData("Aganya - start Program Uart 1 ",USART1);
		char message[100];
		int i =0;
		while(1)
		{
			
			//SendData(message);
			i++;
			if(i>999999)
			{
				sprintf(message,"breakeValue = %d\n\r",breakeValue);
				SendData(message,USART1);
				
				if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8) ==(uint8_t)Bit_RESET)
					GPIO_SetBits(GPIOC,GPIO_Pin_8);
				else
					GPIO_ResetBits(GPIOC,GPIO_Pin_8);
				i=0;
			}
				
		}
	}
	/////////////////////////////////////MAIN//////////////////////////////////
	
	
	void USART1_IRQHandler(void)
{
	
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//	USART_SendData(USART1, 'X');
		char buff[BuffSize]="";
		char temp = 0x00;
		char empty = 0xFF;
		int num = 123;
		char buf[100] = "";
		inBuffIndex=0;

		while(1)
		{
			temp = USART1_Read();
			if(temp != empty)
			{
			buff[inBuffIndex] = temp;
			inBuffIndex++;
			temp = 0x00;
			if(inBuffIndex >= BuffSize)
				break;
			}
			else
				break;
		}
		if(inBuffIndex>0)
		{
			
			strcpy(inBuff,buff);
			switch(mode)
			{
				case 0: // loopback
					SendData(buff,USART1);
					break;
				
				case 1://analyse data				
					// convert 123 to string [buf]
					sprintf(buf,"in pack size %d\n\r",inBuffIndex);
					
					SendData(buf,USART1);
					break;
			}
		}

}  

uint16_t checkMessageID()
{
	//0f1
	if (messageID[0] =='0' && messageID[1] =='F' && messageID[0] =='1')
	{
		calcLogic[0] = 0x02;
		calcLogic[1] = 0x01;
		return True;
	}
	else
	{
		return False;
	}
}

int getValue()
{
	int val =0;
	
	if (messageID[0] =='0' && messageID[1] =='F' && messageID[0] =='1')
	{
		breakeValue = val = (int)messageData[0];
	}
	else
	{
		
	}
	
	return val;
}

void USART2_IRQHandler(void)
{
		char buff[BuffSize]="";
		char temp = 0x00;
		char empty = 0xFF;
		int num = 123;
		char buf[100] = "";
		int inBuffIndex2=0;
		char dateByt[2];
		char infomessage[50]="";
	
	
		char dataLength[1] = "";
		strcpy(messageID,"");
		strcpy(messageData,"");
	
		sprintf(infomessage,"info : Start uart 2 reading \n\r");
		SendData(infomessage,USART1);

		while(1)
		{
			temp = USART2_Read();
			if(temp != empty)
			{
			buff[inBuffIndex2] = temp;
			inBuffIndex2++;
			temp = 0x00;
			if(inBuffIndex2 >= BuffSize)
				break;
			}
			else
				break;
		}
		int i =0;
		char mode =0;
		uint16_t length = 0;
		for(i=0;i<inBuffIndex2;i++)
		{
			temp = buff[i];
			
			switch(mode)
			{
				case 0:
					if(temp=='t')
					{
						mode = 1;
					}
					break;
				case 1:
						//get message ID
						messageID[0] = temp;
						mode = 2;
				break;
				
				case 2:
						//get message ID
						messageID[1] = temp;
						mode = 3;
				break;
				
				case 3:
						//get message ID
						messageID[2] = temp;
						sprintf(infomessage,"info : messageID =  %s\n\r",messageID);
						SendData(infomessage,USART1);
						if(checkMessageID() == True)
							mode = 0;
						else
							mode = 12;
				break;
				
				case 4://0
					dateByt[0] = buff[i];
					i++;
					dateByt[1] = buff[i];
					messageData[0] = atoi(dateByt);
					if((mode - 3)<length)
						mode++;
					else
						mode = 13;
				break;
				
				case 5://1
					dateByt[0] = buff[i];
					i++;
					dateByt[1] = buff[i];
					messageData[1] = atoi(dateByt);
				
					if((mode - 3)<length)
						mode++;
					else
						mode = 13;
				break;
				
				case 6://2
					dateByt[0] = buff[i];
					i++;
					dateByt[1] = buff[i];
					messageData[2] = atoi(dateByt);
				
					if((mode - 3)<length)
						mode++;
					else
						mode = 13;
				break;
				
				case 7://3
					dateByt[0] = buff[i];
					i++;
					dateByt[1] = buff[i];
					messageData[3] = atoi(dateByt);
					if((mode - 3)<length)
						mode++;
					else
						mode = 13;
				break;
				
				case 8://4
					dateByt[0] = buff[i];
					i++;
					dateByt[1] = buff[i];
					messageData[4] = atoi(dateByt);
					if((mode - 3)<length)
						mode++;
					else
						mode = 13;
				break;
				
				case 9://5
					dateByt[0] = buff[i];
					i++;
					dateByt[1] = buff[i];
					messageData[5] = atoi(dateByt);
					if((mode - 3)<length)
						mode++;
					else
						mode = 13;
				break;
				
				case 10://6
					dateByt[0] = buff[i];
					i++;
					dateByt[1] = buff[i];
					messageData[6] = atoi(dateByt);
					if((mode - 3)<length)
						mode++;
					else
						mode = 13;
				break;
				
				case 11://7
					dateByt[0] = buff[i];
					i++;
					dateByt[1] = buff[i];
					messageData[7] = atoi(dateByt);
					mode = 13;
				break;
				
				case 12:
					length = temp&0x0F;
					mode = 4;
					break;
				
				case 13:
					getValue();
					mode = 0;
				break;
			}
				
		}
		
		sprintf(buf,"in pack size %d\n\r",inBuffIndex2);			
		SendData(buf,USART1);
		
		
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
	
	

	


