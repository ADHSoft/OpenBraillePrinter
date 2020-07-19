/*
 * Open Braille Printer (C) 2017-2019 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

#include "AP_Buffer.h"
#include "AP_Funciones.h"
#include "AP_mde_Impresion.h"
#include "PR_motores.h"

#ifndef GOLD_VERSION
#include "free/gold_obp_placeholders.h"
#else
#include "./gold/PR_audio.h"
#include "./gold/PR_UART.h"
#endif

/******************************************************************************************************************
 * @fn		void mde_impresion( bool_t reset )
 * @brief   Maquina de estado encargada de imprimir de a lineas horizontales.
 * @details  void
 * @param:  reset: resetea las variables, ejemplo: numero de renglon actual
 * @return: void
 *****************************************************************************************************************/

void mde_impresion(bool_t reset){

	static int8_t estado=0,letra_actual=1,columna_actual=1;	//columna actual: indica en que columna de la matriz de una letra está
	static int8_t renglon=1;
	bool_t marcar_punto=FALSE;

	if (calibracion_cabezal()==FALSE) return;

	if (reset==TRUE) {
		estado=0;
		letra_actual=1;
		columna_actual=1;
		renglon=1;
		buffer_olvidar_renglon_ya_impreso();
		//dejar el cabezal en la primera letra
		posicionar_motor_y_martillar( POS_X_PRIMERA_LETRA ,  FALSE );
		return;
	}

	switch(estado) {
	case 0:	//manda a imprimir la linea 1 a medida que hay letras por imprimir,
		//o bien si hay salto de linea, lo anota y manda a terminar el renglon
		if ( buffer_get_cola_impresion() == 0 ) {
			return;
		}

		if ( letra_actual==(LETRAS_POR_RENGLON+1 )) { //completó el renglon por falta de espacio.

			letra_actual--;
			columna_actual=2;
			estado=2;
			mover_motor(MOTOR_HOJA,ALTO_ESPACIO_LINEA);
		} else {

			if ( buffer_imprimir() == '\n' ) {
				if (buffer_get_renglon_actual()==1) {	//solo es un salto de linea en todo el renglón
					renglon++;		//no imprimir nada, solo saltar renglón.
					if (renglon>RENGLONES_POR_HOJA) {
						control_de_papel(1,0,0);//Expulsar_hoja
					} else {
						if (renglon>RENGLONES_POR_HOJA-1) reproducir_audio(AUDIO_ULTIMO_RENGLON);	//avisar del ultimo renglon
						mover_motor(MOTOR_HOJA,(ALTO_SEPARACION_RENGLON+ALTO_LETRA));
						//dejar el cabezal en la primera letra
						posicionar_motor_y_martillar( POS_X_PRIMERA_LETRA ,  FALSE );
					}
					estado=0;
					letra_actual=1;
					columna_actual=1;
					buffer_olvidar_renglon_ya_impreso();
				} else {
					columna_actual=2;	//terminar de imprimir el renglón
					estado=2;
					mover_motor(MOTOR_HOJA,ALTO_ESPACIO_LINEA);
				}

			}else {
				estado=1;

			}

		}
		break;
	case 1:	//imprime la primera linea de puntos de las letras que se puedan imprimir al momento


		if (columna_actual == 1) {
			if ( (ascii_a_braille(buffer_leer_renglon_actual(letra_actual-1)) & 0b00000001 ) !=0 ) marcar_punto=TRUE;
		} else {	//col 2
			if ( (ascii_a_braille(buffer_leer_renglon_actual(letra_actual-1)) & 0b00001000 ) !=0 ) marcar_punto=TRUE;
		}
		if ( posicionar_motor_y_martillar( POS_X_PRIMERA_LETRA - ( ANCHO_LETRA*(columna_actual-1) +
				(ANCHO_ESPACIO+ANCHO_LETRA)*(letra_actual-1) ) ,  marcar_punto ) == TRUE ) {
			columna_actual++;	//pasar a siguiente col.
		}
		if (columna_actual==3) {	//letra impresa

			letra_actual++;
			columna_actual=1;
			estado=0;
		}
		break;
	case 2:	//linea 2 (termina de imprimir todo el renglon de corrido)
		if (columna_actual == 1) {
			if ( (ascii_a_braille(buffer_leer_renglon_actual(letra_actual-1)) & 0b00000010 ) !=0 ) marcar_punto=TRUE;
		} else {	//col 2
			if ( (ascii_a_braille(buffer_leer_renglon_actual(letra_actual-1))  & 0b00010000 ) !=0 ) marcar_punto=TRUE;
		}
		if ( posicionar_motor_y_martillar( POS_X_PRIMERA_LETRA - ( ANCHO_LETRA*(columna_actual-1) +
				(ANCHO_ESPACIO+ANCHO_LETRA)*(letra_actual-1) ) ,  marcar_punto ) == TRUE ) {
			columna_actual--;	//pasar a siguiente col.
		}
		if (columna_actual==0) {	//letra impresa
			if (letra_actual>1) {
				letra_actual--;
				columna_actual=2;
			} else {
				estado=3;	//linea 3
				mover_motor(MOTOR_HOJA,ALTO_ESPACIO_LINEA);
				columna_actual=1;
			}
		}
		break;

	case 3:	//linea 3 (termina de imprimir todo el renglon de corrido)
		if (columna_actual == 1) {
			if ( (ascii_a_braille(buffer_leer_renglon_actual(letra_actual-1))  & 0b00000100 ) !=0 ) marcar_punto=TRUE;
		} else {	//col 2
			if ( (ascii_a_braille(buffer_leer_renglon_actual(letra_actual-1))  & 0b00100000 ) !=0 ) marcar_punto=TRUE;
		}

		if ( posicionar_motor_y_martillar( POS_X_PRIMERA_LETRA - ( ANCHO_LETRA*(columna_actual-1) +
				(ANCHO_ESPACIO+ANCHO_LETRA)*(letra_actual-1) ) ,  marcar_punto ) == TRUE ) {
			columna_actual++;	//pasar a siguiente col.
		}
		if (columna_actual==3) {	//letra impresa
			if (letra_actual!=buffer_get_renglon_actual()) {
				letra_actual++;
				columna_actual=1;
			} else {
				buffer_olvidar_renglon_ya_impreso();
				letra_actual=1;
				estado=0;
				columna_actual=1;
				renglon++;

				if (renglon>RENGLONES_POR_HOJA) {
					control_de_papel(1,0,0);//Expulsar_hoja
				} else {
					if (renglon>RENGLONES_POR_HOJA-1) reproducir_audio(AUDIO_ULTIMO_RENGLON);
					mover_motor(MOTOR_HOJA,(ALTO_SEPARACION_RENGLON));
					//dejar el cabezal en la primera letra
					posicionar_motor_y_martillar( POS_X_PRIMERA_LETRA ,  FALSE );
				}

			}
		}
		break;

	}

}
