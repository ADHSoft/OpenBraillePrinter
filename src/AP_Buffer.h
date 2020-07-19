/*
 * Open Braille Printer (C) 2017-2019 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/


#ifndef APLICACION_AP_BUFFER_H_
#define APLICACION_AP_BUFFER_H_

#include "DR_tipos.h"

void buffer_poner_en_cola_impresion();
int8_t buffer_agregar_caracter( int8_t Letra );
int8_t buffer_lleno();
void buffer_olvidar_renglon_ya_impreso();
uint8_t buffer_leer_renglon_actual(uint16_t pos);
uint16_t buffer_get_cola_impresion();
uint16_t buffer_get_renglon_actual();
uint8_t buffer_imprimir();
uint16_t buffer_get_editable();
char* buffer_get_string_editable();
void buffer_borrar_char();
void buffer_cancelar_cola_impresion();
void buffer_borrar_editable();

#endif /* APLICACION_AP_BUFFER_H_ */
