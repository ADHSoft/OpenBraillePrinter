/*
 * Open Braille Printer (C) 2017-2019 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

#ifndef APLICACION_AP_FUNCIONES_H_
#define APLICACION_AP_FUNCIONES_H_

#include "DR_tipos.h"

uint8_t ascii_a_braille( char letra_ascii );
bool_t control_de_papel(int8_t retirar,int8_t ingresar,int8_t *Modo);
void delay_s(int sec);
uint8_t braille_to_ascii( char letra_braille );

#endif /* APLICACION_AP_FUNCIONES_H_ */
