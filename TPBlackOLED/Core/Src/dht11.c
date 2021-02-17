/*
 * dht11.c
 *
 *  Created on: 1 de fev de 2021
 *      Author: cladi
 */

#include "dht11.h"
#include "stdint.h"

extern TIM_HandleTypeDef htim1; //Modifique de acordo com seu código
#define htim htim1

extern DHT11_struct Dstruct;


//Funcoes auxiliares
void delay_us(uint16_t us){
	__HAL_TIM_SET_COUNTER(&htim,0);  // Seta counter para 0
		while (__HAL_TIM_GET_COUNTER(&htim) < us); //espera
}
void Set_Output(GPIO_TypeDef *port, uint16_t pin){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}
void Set_Input(GPIO_TypeDef *port, uint16_t pin){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}

uint8_t DHT11_setup (){
	//inicio da comunicação, manda 0 por 18ms
			Set_Output(Dstruct.port, Dstruct.pin);
			HAL_GPIO_WritePin (Dstruct.port, Dstruct.pin, 1);
			delay_us(1000);
			HAL_GPIO_WritePin (Dstruct.port, Dstruct.pin, 0);
			delay_us(20000);
			HAL_GPIO_WritePin (Dstruct.port, Dstruct.pin, 1);
			delay_us(20);
			Set_Input(Dstruct.port, Dstruct.pin);

		//espera resposta do sensor (sensor manda 0 por 80us e em seguida 1 por 80us)
			uint8_t DHTStatus = 0;
			delay_us(40);
			if (!(HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin))){
				delay_us(80);
				if ((HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin))) DHTStatus = 1;
				else return 0;
				}
			while ((HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin)));   // espera ir pra 0
			return 1;
}


//Inicilização
void DHT11_init(GPIO_TypeDef* port, uint16_t pin){
	Dstruct.port = port;
	Dstruct.pin = pin;
}


//Funções API
uint8_t DHT11_rawread(){
	if(DHT11_setup()){
	//recebe  5 valores de 8bits (bit '0' High por 26us e bit '1' High 1 por 80us)
	uint8_t i,j, value = 0;
	uint8_t values[5];
	for(j=0;j<5;j++){
		for (i=0;i<8;i++){
			while (!(HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin)));   //espera ir pra 0
			delay_us(47);   											//espera 47us
			if (!(HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin))){		//se tiver em 0,bit lido '0'
				value &= ~(1<<(7-i));
			}
			else value|= (1<<(7-i));  									//se tiver em 1, bit lido '1'
			while ((HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin))); 	// espera voltar pra 0
		}
		values[j] = value;
	}
	Dstruct.rhInt = values[0];
	Dstruct.rhDec = values[1];
	Dstruct.tempInt = values[2];
	Dstruct.tempDec = values[3];
	Dstruct.checksum = values[4];

		/*if(!(Dstruct.checksum == Dstruct->temperature + Dstruct->humidity)){
			return -1;
		} else return 1;*/
		return 1;
	}
	else return 0;

}

float DHT11_tempInt(){
	uint8_t values[4];
	float temp;
	if(DHT11_setup()){
		//recebe  5 valores de 8bits (bit '0' High por 26us e bit '1' High 1 por 80us)
		uint8_t i,j, value = 0;
		for(j=0;j<4;j++){
			for (i=0;i<8;i++){
				while (!(HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin)));   //espera ir pra 0
				delay_us(47);   											//espera 47us
				if (!(HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin))){		//se tiver em 0,bit lido '0'
					value &= ~(1<<(7-i));
				}
				else value|= (1<<(7-i));  									//se tiver em 1, bit lido '1'
				while ((HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin))); 	// espera voltar pra 0
			}
			values[j] = value;
		}
		temp = values[2] + (float)values[3] / 10;
		return temp;
	}
	else return 0;
}

uint8_t DHT11_rh(){
	uint8_t value;
	if(DHT11_setup()){
		//recebe  5 valores de 8bits (bit '0' High por 26us e bit '1' High 1 por 80us)
		uint8_t i;
			for (i=0;i<8;i++){
				while (!(HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin)));   //espera ir pra 0
				delay_us(47);   											//espera 47us
				if (!(HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin))){		//se tiver em 0,bit lido '0'
					value &= ~(1<<(7-i));
				}
				else value|= (1<<(7-i));  									//se tiver em 1, bit lido '1'
				while ((HAL_GPIO_ReadPin (Dstruct.port, Dstruct.pin))); 	// espera voltar pra 0
			}
	}
	return value;
}

float DHT11_dewpoint(){
	float gama,dp;
	gama = log(Dstruct.rhInt/100.0) + (17.67*Dstruct.tempInt)/(243.5+Dstruct.tempInt);
	dp = (243.5*gama)/(17.67-gama);
	return dp;
}

float DHT11_ah(){
	float aux, ah;
	aux = exp((17.67*Dstruct.tempInt)/(243.5+Dstruct.tempInt));
	ah = 6.112*aux*Dstruct.rhInt*2.1674 / (273.15+Dstruct.tempInt);
	return ah;
}

