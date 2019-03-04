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
char inBuff1[BuffSize];
char inBuff2[BuffSize];
int inBuffIndex = 0; 
int inBuffIndex2 = 0; 
int readIndex = 0; 
int readIndex2 = 0; 
char mode =0;
uint16_t tempReciveByte;
uint16_t length = 0;
char dataByte[2];
uint16_t dataByte_flag = False;

int Uart2Budrate = 57600;
char calcLogic[2] = "00";
char messageData[8] = "";
char messageID[3]="";
int breakeValue = 0;
int breakLevel[6] = {30,60,90,120,150,170};
char setOutput = 0;


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

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | 
																GPIO_Pin_2 | GPIO_Pin_3 |
																GPIO_Pin_4 | GPIO_Pin_5 |
																GPIO_Pin_6 | GPIO_Pin_7 |
																GPIO_Pin_8 | GPIO_Pin_9;
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

uint16_t readUart(USART_TypeDef* USARTx)
{
	
		tempReciveByte = 0xFFFF;
		char dummyarry[1];
		if (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) != RESET)
			{
				if(USARTx == USART1)
				{tempReciveByte = USART1_Read();}
				else
				{tempReciveByte = USART2_Read();}
			
				if(tempReciveByte != 0xFFFF)
				{
					//tempReciveByte = USART1_Read();
//					inBuff1[inBuffIndex] = tempReciveByte;
					dummyarry[0] =	tempReciveByte;
					SendData(dummyarry,USARTx);
					return True;
				}
			}
	return False; 
}

void reciveCanMessage();
void Set_BreakOutput(char);
//void InfoMessage(char *message)
//{
//	char buf[100];
//	sprintf(buf,"*************breake value = %d\n\r",breakeValue);
//	SendData(buf,USART1);
//}

void ggg()
{
		
		if((int)(setOutput) != 0)
		{
			GPIO_SetBits(GPIOC,GPIO_Pin_0);
		}
		else
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_0);
		}
		
		if((int)(setOutput) > 30)
		{
			GPIO_SetBits(GPIOC,GPIO_Pin_1);
		}
		else
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_1);
		}
		
		if((int)(setOutput) > 60)
		{
			GPIO_SetBits(GPIOC,GPIO_Pin_2);
		}
		else
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_2);
		}
		if((int)(setOutput) > 90)
		{
			GPIO_SetBits(GPIOC,GPIO_Pin_3);
		}
		else
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_3);
		}
		if((int)(setOutput) > 120)
		{
			GPIO_SetBits(GPIOC,GPIO_Pin_4);
		}
		else
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_4);
		}
		if((int)(setOutput) > 150)
		{
			GPIO_SetBits(GPIOC,GPIO_Pin_5);
		}
		else
		{
			GPIO_ResetBits(GPIOC,GPIO_Pin_5);
		}
}

/////////////////////main//////////////////////////////////////////////main/////////////////////////
int main(void) 
	{
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
		
		InitGPIOs();
		InitUSART1();
		InitUSART2();
//		GPIO_Init(GPIOC,
//		GPIO_DeInit(GPIOC);
		GPIO_WriteBit(GPIOC, GPIO_Pin_5, Bit_SET);
//		GPIO_SetBits(GPIOC,GPIO_Pin_8);
//		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
		SendData("Aganya - start Program Uart 2 ",USART2);
		SendData("Aganya - start Program Uart 1 ",USART1);
		char message[100];
		int i =0;
		int j = 1999;
		char out=0x00;
		
		while(1)
		{
			//SendData(message);
			//read from uart1
			if(readIndex!=inBuffIndex)
			{
				if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9) ==(uint8_t)Bit_RESET)
					GPIO_SetBits(GPIOC,GPIO_Pin_9);
				else
					GPIO_ResetBits(GPIOC,GPIO_Pin_9);
				
				
			}
			
			//recive UART2
			if(readIndex2 != inBuffIndex2)
			{
				reciveCanMessage();
			}
			
			i++;
			if(i>j)
			{
				j-=100;
//				sprintf(message,"breakeValue = %d\n\r",breakeValue);
//				SendData(message,USART1);
				//Set_BreakOutput(out);
				
				if((int)setOutput<250)
					setOutput++;
				else
					setOutput =0;
				
				if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9) ==(uint8_t)Bit_RESET)
					GPIO_SetBits(GPIOC,GPIO_Pin_9);
				else
					GPIO_ResetBits(GPIOC,GPIO_Pin_9);	
				
				i=0;
			}
			if (j<200)
				j = 49999;
			
			
				
			ggg();
		}
		
		
	}
/////////////////////main//////////////////////////////////////////////main/////////////////////////	
void USART1_IRQHandler(void)
	{
	
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//	USART_SendData(USART1, 'X');
//		char buff[BuffSize]="";
		char temp = 0x00;
		uint16_t empty = 0xFFFF;
//		int num = 123;
//		char buf[100] = "";
//		inBuffIndex=0;

		while(1)
		{
			temp = USART1_Read();
			if(temp != empty)
			{
				inBuff1[inBuffIndex] = temp;
				inBuffIndex++;
				temp = 0x00;
				if(inBuffIndex >= BuffSize)
					inBuffIndex=0;
			}
			else
				break;
		}
//		if(inBuffIndex>0)
//		{
//			
//			strcpy(inBuff1,buff);
//			switch(mode)
//			{
//				case 0: // loopback
//					SendData(buff,USART1);
//					break;
//				
//				case 1://analyse data				
//					// convert 123 to string [buf]
//					sprintf(buf,"in pack size %d\n\r",inBuffIndex);
//					
//					SendData(buf,USART1);
//					break;
//			}
//		}

}  

void USART2_IRQHandler(void)
{
	
//	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//	USART_SendData(USART1, 'X');
//		char buff[BuffSize]="";
		uint16_t temp = 0x00;
		uint16_t empty = 0xFFFF;
//		int num = 123;
//		char buf[100] = "";
		//inBuffIndex2=0;

		while(1)
		{
			temp = USART2_Read();
			if(temp != empty)
			{
				inBuff2[inBuffIndex2] = temp & 0x00FF;
				inBuffIndex2++;
				
				if(inBuffIndex2 >= BuffSize)
					inBuffIndex2=0;
				
				temp = 0x00;
				
			}
			else
				break;
		}
}
uint16_t checkMessageID()
{
	//0f1
	char ID_0F1[] = "0F1";
	if (strcmp(messageID, ID_0F1) == 0)
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
		//0f1
	char ID_0F1[] = "0F1";
	if (strcmp(messageID, ID_0F1) == 0)
	{
		breakeValue = val = (int)messageData[0];
	}
	else
	{
		
	}
	
	return val;
}

void CanDataMessageRecord(int length,int dataLoc,char currentChar)
{

	if(dataByte_flag == False)
		{
			dataByte_flag = True;
			dataByte[0] =  currentChar;
			
		}
		else
		{
			dataByte_flag = False;	
			dataByte[1] = currentChar;// & 0x0F;
			if(dataLoc < (length - 1))
				mode++;
			else
				mode = 13;
			messageData[dataLoc] = (int)strtoul(dataByte, NULL, 16); //atoi(dataByte); //dataByte[0] + dataByte[1]; // (char)atoi(dataByte);
		}
}
	

void reciveCanMessage(void)
{
		int i =0;
		
		char buf[100];	
		char currentChar = inBuff2[readIndex2];
		readIndex2++;
	
		if(readIndex2 >= BuffSize)
			readIndex2=0;
			switch(mode)
			{
				case 0:
					length = 0;
					messageID[0] = 0;
					messageID[1] = 0;
					messageID[2] = 0;
					messageData[0] = 0;
					messageData[1] = 0;
					messageData[2] = 0;
					messageData[3] = 0;
					messageData[4] = 0;
					messageData[5] = 0;
					messageData[6] = 0;
					messageData[7] = 0;
					
					if(currentChar=='t')
					{
						mode = 1;
					}
					break;
				case 1:
						//get message ID
						messageID[0] = currentChar;
						mode = 2;
				break;
				
				case 2:
						//get message ID
						messageID[1] = currentChar;
						mode = 3;
				break;
				
				case 3:
						//get message ID
						messageID[2] = currentChar;
						if(checkMessageID() != True)
							mode = 0;
						else
							mode = 12;
						
						sprintf(buf,"************* MessageID = %s\n\r",messageID);
						SendData(buf,USART1);
				break;
				
				case 4://0
					CanDataMessageRecord(length,0,currentChar);
				break;
				
				case 5://1
					CanDataMessageRecord(length,1,currentChar);
				break;
				
				case 6://2
					CanDataMessageRecord(length,2,currentChar);
				break;
				
				case 7://3
					CanDataMessageRecord(length,3,currentChar);
				break;
				
				case 8://4
					CanDataMessageRecord(length,4,currentChar);
				break;
				
				case 9://5
					CanDataMessageRecord(length,5,currentChar);
				break;
				
				case 10://6
					CanDataMessageRecord(length,6,currentChar);
				break;
				
				case 11://7
				CanDataMessageRecord(length,7,currentChar);
				mode = 13;
				break;
				
				case 12:
					length = currentChar&0x0F;
					length--;
					mode = 4;
					break;
				
				default:
					mode = 0;
			}
			
			if (mode == 13)
			{				
				getValue();
				mode = 0;
				sprintf(buf,"*************breake value = %d\n\r",breakeValue);
				SendData(buf,USART1);
			
				if (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9) ==(uint8_t)Bit_RESET)
					GPIO_SetBits(GPIOC,GPIO_Pin_9);
				else
					GPIO_ResetBits(GPIOC,GPIO_Pin_9);	
			}
		
}
