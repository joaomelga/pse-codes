/*
 * dht11.h
 *
 *  Created on: 1 de fev de 2021
 *      Author: cladi
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "stm32f4xx_hal.h" //Ajuste para sua família OBS.: BlackBoard com Cortex M4 tem floating point unit, n sei bluepill
#include "stdint.h"

typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
	uint8_t tempInt;
	uint8_t tempDec;
	uint8_t rhInt;
	uint8_t rhDec;
	uint8_t checksum;
} DHT11_struct;

//-------Funções API---------
void DHT11_init(GPIO_TypeDef* port, uint16_t pin); //configura a porta e o pino
uint8_t DHT11_rawread(); //preenche o struct, retorna 1 se checksum OK, 0 se NOK
					 	 //OBS.: Somando todos os tempos no pior caso de leitura possivel - 40bits '1' -
					 	 //o tempo máximo de leitura é de 26ms
						// Contudo, tempo de amostragem deve ser de no mínimo 1s
float DHT11_tempInt();  //retorna temperatura em °C ou 0 caso falha
uint8_t DHT11_rh();   //retorna umidade relativa do ar em % ou 0 caso falha
float DHT11_dewpoint(); //retorna ponto de orvalho em °C pela fórmula de Magnus, da ultima leitura feita
float DHT11_ah();       //retorna umidade absoluta em g/m³ da ultima leitura feita
						//https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/

#endif /* INC_DHT11_H_ */
