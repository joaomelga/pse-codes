/*
 * dht11.h
 *
 *  Created on: 1 de fev de 2021
 *      Author: cladi
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "stm32f4xx_hal.h" //só pra familia f4

typedef struct DHT11_struct {
	GPIO_TypeDef* port;
	uint16_t pin;
	uint16_t temperature;
	uint16_t humidity;
	uint8_t checksum;
} DHT11_struct;


//deve-se configurar Timer no main
//no nosso caso, TIM1 com 16Mhz
//htim1.Instance = TIM1;
//htim1.Init.Prescaler = 16-1;
//htim1.Init.CounterMode = 0x00000000U;
//htim1.Init.Period = 0xfff-1;
void delay_1us(TIM_HandleTypeDef *htim, uint16_t us);


//mudar para uint8_t dps
//a configuração do port é feita na funcao, usuario nao precisa de antemao
int DHT11_init(struct DHT11_struct* Dstruct,GPIO_TypeDef* port, uint16_t pin);
int DHT11_rawread(struct DHT11_struct* Dstruct, TIM_HandleTypeDef *htim); //retorna 1 se checksum OK, -1 se NOK

#endif /* INC_DHT11_H_ */
