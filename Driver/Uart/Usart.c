/*
 * This file is part of the stm32 tic-tac-toe game (https://github.com/Sisoog/stm32_tictactoc_game).
 * Copyright (c) 2021 mohammad mazarei.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stm32f0xx.h>
#include <xprintf/xprintf.h>

#define RX_BUFFER_SIZE	16

volatile char rx_buffer[RX_BUFFER_SIZE];
 unsigned int rx_wr_index=0,rx_rd_index=0;
 unsigned int rx_counter=0;

void USART1_IRQHandler(void)
{

  //if((USART1->CR1 & USART_CR1_RXNEIE) && (USART1->ISR & USART_ISR_RXNE))
  //if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
	//  char data = USART_ReceiveData(USART1);
   char data = USART1->RDR;
   if(data=='\r')
	   data = '\n';

   rx_buffer[rx_wr_index++]=data;
   if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
   if (++rx_counter == RX_BUFFER_SIZE)
   {
     rx_counter=0;
   }
  }
}

unsigned char Uart1_GetBlock(void)
{
	unsigned char data;
	while (rx_counter==0)
		__NOP();

	data=rx_buffer[rx_rd_index++];
	if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
	__disable_irq();
	--rx_counter;
	__enable_irq();
	//Uart1_Send(data);
	return data;
}

void Send_Byte (unsigned char c)
{
    while ((USART1->ISR & USART_FLAG_TXE) == 0);
    USART1->TDR = c;
}

void Usart_init(uint32_t Baud)
{
#define GPIO_2	2
#define GPIO_3	3

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	GPIOA->AFR[GPIO_PinSource2 >> 0x03] |= ((uint32_t)(GPIO_AF_1) << ((uint32_t)((uint32_t)GPIO_PinSource2 & (uint32_t)0x07) * 4));
	GPIOA->AFR[GPIO_PinSource3 >> 0x03] |= ((uint32_t)(GPIO_AF_1) << ((uint32_t)((uint32_t)GPIO_PinSource3 & (uint32_t)0x07) * 4));

	GPIOA->OSPEEDR |= ((uint32_t)(GPIO_Speed_50MHz) << (GPIO_2 * 2));
	//GPIOA->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)GPIO_2));
	GPIOA->MODER |= (((uint32_t)GPIO_Mode_AF) << (GPIO_2 * 2));
	GPIOA->PUPDR |= (((uint32_t)GPIO_PuPd_UP) << (GPIO_2 * 2));

	GPIOA->OSPEEDR |= ((uint32_t)(GPIO_Speed_50MHz) << (GPIO_3 * 2));
	//GPIOA->OTYPER |= (uint16_t)(((uint16_t)GPIO_OType_PP) << ((uint16_t)GPIO_3));
	GPIOA->MODER |= (((uint32_t)GPIO_Mode_AF) << (GPIO_3 * 2));
	GPIOA->PUPDR |= (((uint32_t)GPIO_PuPd_UP) << (GPIO_3 * 2));

	/* USARTx configured as follow:
	  - BaudRate = 115200 baud
	  - Word Length = 8 Bits
	  - Stop Bit = 1 Stop Bit
	  - Parity = No Parity
	  - Hardware flow control disabled (RTS and CTS signals)
	  - Receive and transmit enabled
	*/
	  USART1->CR1 |= (uint32_t)(USART_WordLength_8b | USART_Parity_No | (USART_Mode_Tx | USART_Mode_Rx));
	  USART1->CR2 |= (uint32_t)USART_StopBits_1;

	  /*---------------------------- USART BRR Configuration -----------------------*/
	  /* Configure the USART Baud Rate -------------------------------------------*/

	  uint32_t divider = 0, tmpreg = 0;
	  /* (divider * 10) computing in case Oversampling mode is 16 Samples */
	  divider = (uint32_t)((SystemCoreClock) / (Baud));
	  tmpreg  = (uint32_t)((SystemCoreClock) % (Baud));

	  /* round the divider : if fractional part i greater than 0.5 increment divider */
	  if (tmpreg >=  (Baud) / 2)
	    divider++;

	  /* Write to USART BRR */
	  USART1->BRR = (uint16_t)divider;

	  /* Enable USARTy Receive and Transmit interrupts */
	  USART1->CR1 |= 32;

	  /* Enable the selected USART by setting the UE bit in the CR1 register */
	  USART1->CR1 |= USART_CR1_UE;

	  /* Compute the Corresponding IRQ Priority --------------------------------*/
	  NVIC->IP[USART1_IRQn >> 0x02] = 0;

	  /* Enable the Selected IRQ Channels --------------------------------------*/
	  NVIC->ISER[0] = (uint32_t)0x01 << (USART1_IRQn & (uint8_t)0x1F);

	  xdev_out(Send_Byte);
	  xdev_in(Uart1_GetBlock);
}



