/*
 * Open Braille Printer (C) 2017-2019 Alejandro Herme.
 * See LICENSE.txt for more details. Contact : alejandrodherme@gmail.com
*/

#include "DR_tipos.h"
#include "AP_Buffer.h"
#include "PR_motores.h"
#define TAMANO_BUFFER 720

static char Buffer[TAMANO_BUFFER];

__RW uint8_t comienzo=0;
// El Buffer tiene tres zonas en este orden:
__RW uint8_t cantidad_letras_imprimiendo_renglon_actual=0;	//son los caracteres desde el primero que se imprimió en el renglon hasta el salto de linea
														//(si es que ya se lo insertó).
__RW uint8_t cantidad_cola_impresion=0;	//son el resto de caracteres que se mandaron a imprimir.
__RW uint8_t cantidad_editable=0;	//lo que no se mandó a imprimir todavía, para poder corregirlo o cambiarlo.


int8_t buffer_lleno() {
	if (cantidad_cola_impresion+cantidad_letras_imprimiendo_renglon_actual+cantidad_editable == TAMANO_BUFFER )
		return 1;
	else return 0;
}

void buffer_poner_en_cola_impresion(){
	cantidad_cola_impresion+=cantidad_editable;
	cantidad_editable=0;
}

void buffer_borrar_editable(){
	cantidad_editable=0;
}

int8_t buffer_agregar_caracter( int8_t Letra ) {

	if (buffer_lleno()==0) {

		Buffer[(comienzo+cantidad_letras_imprimiendo_renglon_actual
				+cantidad_editable+cantidad_cola_impresion)%TAMANO_BUFFER]=Letra;
		cantidad_editable++;

		return 0;
	} else {
		return -1;
	}

}

void buffer_olvidar_renglon_ya_impreso(){
	comienzo=(comienzo+cantidad_letras_imprimiendo_renglon_actual)%TAMANO_BUFFER;
	cantidad_letras_imprimiendo_renglon_actual=0;
}

uint8_t buffer_leer_renglon_actual(uint16_t pos){
	return Buffer[(comienzo+pos)%TAMANO_BUFFER];
}

uint16_t buffer_get_cola_impresion(){
	return cantidad_cola_impresion;
}

uint16_t buffer_get_editable(){
	return cantidad_editable;
}

/******************************************************************************************************************
 * @fn		char* buffer_get_string_editable( void )
 * @brief   devuelve el texto imprimible para el lcd que muestra la ultima parte del texto ingresado
 * @details  void
 * @param:  void
 * @return: string para poder imprimirlo en pantalla
 *****************************************************************************************************************/
char* buffer_get_string_editable(){	//lo necesita el LCD para imprimirlo
	#define TAMANO_PANTALLA 32
	static char str[TAMANO_PANTALLA+1];
	uint8_t pos=0;

	if (cantidad_editable< TAMANO_PANTALLA ) {	//si el buffer entra en la pantalla
		for ( pos=0;pos<cantidad_editable;pos++ ){
			str[pos]=Buffer[(comienzo+cantidad_letras_imprimiendo_renglon_actual+cantidad_cola_impresion+pos)%TAMANO_BUFFER];
		}
		str[pos]='\0';
	} else {	//sino, llenar pantalla solo con la ultima parte del buffer.
		for (pos=0;pos<(TAMANO_PANTALLA-1);pos++){
			str[pos]=Buffer[(comienzo+cantidad_letras_imprimiendo_renglon_actual+cantidad_cola_impresion
					+pos+(cantidad_editable-TAMANO_PANTALLA+1))%TAMANO_BUFFER];
		}
	}

	str[TAMANO_PANTALLA]='\0';

	return str;
}

uint16_t buffer_get_renglon_actual(){
	return cantidad_letras_imprimiendo_renglon_actual;
}

void buffer_cancelar_cola_impresion(){
	cantidad_editable+=cantidad_cola_impresion;	//mover
	cantidad_cola_impresion=0;
}

uint8_t buffer_imprimir(){
	//mover de cola a renglon actual

	cantidad_cola_impresion--;
	cantidad_letras_imprimiendo_renglon_actual++;

	//devolver la nueva letra que se agrega al renglon
	return Buffer[(comienzo+cantidad_letras_imprimiendo_renglon_actual-1)%TAMANO_BUFFER];
}

void buffer_borrar_char(){
	if (cantidad_editable!=0) cantidad_editable--;
}
