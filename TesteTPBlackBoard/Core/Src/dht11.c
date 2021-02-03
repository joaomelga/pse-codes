/*
 * dht11.c
 *
 *  Created on: 1 de fev de 2021
 *      Author: cladi
 */

#include "dht11.h"



//Funcoes auxiliares
void delay_us(TIM_HandleTypeDef *htim, uint16_t us){
	  //htim->Init->Prescaler = 16-1;
	  //htim->Init->CounterMode = 0x00000000U;
	  //htim->Init->Period = 0xfff-1;
	  //HAL_TIM_Base_Start_IT(&htim);

	//verificar passagem de struct por referencia
	__HAL_TIM_SET_COUNTER(htim,0);  // Seta counter para 0
		while (__HAL_TIM_GET_COUNTER(htim) < us); //espera
}

void Set_Output(GPIO_TypeDef *port, uint16_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void Set_Input(GPIO_TypeDef *port, uint16_t pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}


int DHT11_init(struct DHT11_struct* Dstruct,GPIO_TypeDef* port, uint16_t pin){

	Dstruct->port = port;
	Dstruct->pin = pin;

	return 0;
}

int DHT11_rawread(struct DHT11_struct* Dstruct, TIM_HandleTypeDef *htim){
	//inicio da comunicação, manda 0 por 18ms
		Set_Output(Dstruct->port, Dstruct->pin);
		HAL_GPIO_WritePin (Dstruct->port, Dstruct->pin, 0); //pino pra 0
		delay_us(htim,18000);									//espera 18ms
		HAL_GPIO_WritePin (Dstruct->port, Dstruct->pin, 1); //pino pra 1
		delay_us (htim,20);   									//espera 20us
		Set_Input(Dstruct->port, Dstruct->pin);

	//espera resposta do sensor (sensor manda 0 por 80us e em seguida 1 por 80us)
		uint8_t DHTStatus = 0;
		delay_us(htim,40);
		if (!(HAL_GPIO_ReadPin (Dstruct->port, Dstruct->pin))){
			delay_us(htim,80);
			if ((HAL_GPIO_ReadPin (Dstruct->port, Dstruct->pin))) DHTStatus = 1;
			else DHTStatus = -1;
			}
		while ((HAL_GPIO_ReadPin (Dstruct->port, Dstruct->pin)));   // espera ir pra 0

	//recebe  5 valores de 8bits (bit '0' High por 26us e bit '1' High 1 por 80us)
		uint8_t i,j, value = 0;
		uint8_t values[5];
		for(j=0;j<5;j++){
			for (i=0;i<8;i++){
				while (!(HAL_GPIO_ReadPin (Dstruct->port, Dstruct->pin)));   //espera ir pra 1
				delay_us(htim, 27);   											//espera 27us
				if (!(HAL_GPIO_ReadPin (Dstruct->port, Dstruct->pin))){		//se tiver em 0,bit lido '0'
					value &= ~(1<<(7-i));
					//experimentar mudar nd
				}
				else value|= (1<<(7-i));  									//se tiver em 1, bit lido '1'
				while ((HAL_GPIO_ReadPin (Dstruct->port, Dstruct->pin))); 	// espera voltar pra 0
			}
			values[j] = value;
		}
		Dstruct->temperature = (values[0]<<8 | values[1]);
		Dstruct->humidity = (values[2]<<8 | values[3]);
		Dstruct->checksum = values[4];

		if(!(Dstruct->checksum == Dstruct->temperature + Dstruct->humidity)){
			return -1;
		} else return 1;

}


