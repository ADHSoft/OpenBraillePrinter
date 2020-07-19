/*
 * Open Braille Printer (C) 2017-2019 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

#include "AP_Funciones.h"
#include "PR_lcd.h"
#include "PR_debounce.h"
#include "PR_motores.h"
#include "AP_Buffer.h"
#include "PR_Timers.h"
#include "AP_mde_Impresion.h"

#ifndef GOLD_VERSION
#include "free/gold_obp_placeholders.h"
#else
#include "./gold/PR_audio.h"
#endif

#define BUFFER		0
#define INMEDIATO	1
#define PC			2

/******************************************************************************************************************
 * @fn      int16_t ascii_a_braille( char letra_ascii )
 * @brief   Convierte un caracter ascii a una letra braille
 * @param:  letra_ascii
 * @return: Un byte donde sus bits son los puntos del dibujo de la letra braille
 *****************************************************************************************************************/
uint8_t ascii_a_braille( char letra_ascii )
{
	uint8_t letrabraille=0;

	if (letra_ascii >= 'A' && letra_ascii <= 'Z') letra_ascii+=0x20;	//pasar mayusculas a minusculas

	switch( letra_ascii )
	{
		case '1':
		case 'a': letrabraille = 0b00000001;	break;
		case '2':
		case 'b': letrabraille = 0b00000011;    break;
		case '3':
		case 'c': letrabraille = 0b00001001;    break;
		case '4':
		case 'd': letrabraille = 0b00011001;	break;
		case '5':
		case 'e': letrabraille = 0b00010001;    break;
		case '6':
		case 'f': letrabraille = 0b00001011;	break;
		case '7':
		case 'g': letrabraille = 0b00011011;	break;
		case '8':
		case 'h': letrabraille = 0b00010011;	break;
		case '9':
		case 'i': letrabraille = 0b00001010;	break;
		case '0':
		case 'j': letrabraille = 0b00011010;	break;

		case 'k': letrabraille = 0b00000101;	break;
		case 'l': letrabraille = 0b00000111;	break;
		case 'm': letrabraille = 0b00001101;	break;
		case 'n': letrabraille = 0b00011101;	break;
		case 'o': letrabraille = 0b00010101;	break;
		case 'p': letrabraille = 0b00001111;	break;
		case 'q': letrabraille = 0b00011111;	break;
		case 'r': letrabraille = 0b00010111;	break;
		case 's': letrabraille = 0b00001110;	break;
		case 't': letrabraille = 0b00011110;    break;
		case 'u': letrabraille = 0b00100101;	break;
		case 'v': letrabraille = 0b00100111;	break;
		case 'w': letrabraille = 0b00111010;	break;
		case 'x': letrabraille = 0b00101101;	break;
		case 'y': letrabraille = 0b00111101;	break;
		case 'z': letrabraille = 0b00110101;	break;

		case  0xEE : letrabraille = 0b00111011;	break;	//ñ

		case '^': letrabraille = 0b00101000;	break;	//mayuscula
		case '#': letrabraille = 0b00111100;	break;	//numero

		case ',': letrabraille = 0b00000010;	break;
		case '.': letrabraille = 0b00000100;	break;
		case '_': letrabraille = 0b00100100;	break; // Tecla a la derecha del "."
		case '?': letrabraille = 0b00100010;	break; // Tecla a la derecha del "0"
		case ';': letrabraille = 0b00000110;	break;
		case '!': letrabraille = 0b00010110;	break; // Tecla a la izquierda del "retroceso"
		case '(': letrabraille = 0b00100011;	break; // Tecla a la derecha de la "P"
		case ')': letrabraille = 0b00011100;	break; // Tecla a la derecha de la "Ñ"
		case '"': letrabraille = 0b00100110;	break; // Tecla a la izquierda del "1"
		case '*': letrabraille = 0b00010100;	break; // Tecla a la izquierda del enter
		default:
		case '\n':
		case ' ': letrabraille = 0;				break;
	}

	return letrabraille;// cambiar nombre a codigo_braille
}

uint8_t braille_to_ascii( char codigo_braille ){
	uint16_t i;
	uint8_t c;

	if (codigo_braille==0) return ' ';
	//try converting every character until we guess which one was intended.
	for (i=0 ; i<256 && c!=0 ; i++) {
		if (ascii_a_braille(i)==codigo_braille) c=i;

	}
	if (c==0) {	//character is not on list. Turn on its first two bits.
		codigo_braille|=0b11000000;
		return codigo_braille;
	} else {
		if (c >= 'A' && c <= 'Z') c+=0x20;	//pasar mayusculas a minusculas
		if (c >= '0' && c <= '9') c+=0x30;	//pasar numero a letra minuscula
		return c;
	}



}

void delay_s(int sec) { // delay in seconds

	#define	EV_DELAY	20

	TimerStart(EV_DELAY,sec*10+1,DEC,NULL);
	while(TimerGet(EV_DELAY)!=0) {};
	TimerStop(EV_DELAY);
}

/******************************************************************************************************************
 * @fn		void Control_de_Papel( void )
 * @brief   Realiza la tarea de retirar una hoja y tomar una nueva.
 * @details  Controla el cambio en el sensor de papel, para detener la impresion.
 * @param:  void
 * @return: bool_t Papel y modo.
 *****************************************************************************************************************/
bool_t control_de_papel(int8_t retirar,int8_t ingresar,int8_t *Modo){	//los dos en 0 para actualizar variable Papel.

	static int8_t Expulsando_hoja=0,Ingresando_hoja=0;
	static bool_t Papel=FALSE;

	if (retirar) {
		Expulsando_hoja=1;
		lcdprintf("Expulsando Hoja...","");
		Papel=FALSE;
		if (calibracion_cabezal()==TRUE) mde_impresion(TRUE);	//posicionar cabezal en la 1era letra
	}
	if (ingresar) {
		Ingresando_hoja=1;
		lcdprintf("Recibiendo Hoja...","");
		Papel=FALSE;
		if (calibracion_cabezal()==TRUE) mde_impresion(TRUE);	//posicionar cabezal en la 1era letra
	}

	if ( !ingresar && !retirar ) {	//
		if (Expulsando_hoja || Ingresando_hoja) {	//pendiente retirar o ingresar
			if (Expulsando_hoja) {
				if (SENSOR_PAPEL_DEBOUNCED) {	//seguir retirando
					if (motor_listo(MOTOR_HOJA)) mover_motor(MOTOR_HOJA,50);
					Papel=FALSE;
				} else {			//hoja retirada
					mover_motor(MOTOR_HOJA,ALTO_LETRA*5);
					Expulsando_hoja=0;
					lcdprintf("Ingrese nueva","hoja.");
					Papel=FALSE;
				}
			}
			if (Ingresando_hoja) {
				if (!SENSOR_PAPEL_DEBOUNCED) {	//seguir ingresando
					if (motor_listo(MOTOR_HOJA) == TRUE) mover_motor(MOTOR_HOJA,10);
					Papel=FALSE;
				} else {		//hoja ingresada
					mover_motor(MOTOR_HOJA,ALTO_MARGEN_SUP);
					Ingresando_hoja=0;
					lcdprintf("Hoja cargada.","");
					reproducir_audio(AUDIO_HOJA_CARGADA);
					Papel=TRUE;
				}
			}
		} else {	//actualizar variable Papel

			if(Papel==TRUE) {	//Si se pierde el papel repentinamente:
				if (!SENSOR_PAPEL_DEBOUNCED) {
					Papel=FALSE;
					buffer_olvidar_renglon_ya_impreso();
					mde_impresion(TRUE);	//resetear MdE
					detener_audio();
					reproducir_audio(AUDIO_NO_HAY_HOJA);
					lcdprintf("Hoja Perdida.","Inserte otra.");

					if (*Modo==INMEDIATO) {	//parar el modo inmediato
						*Modo=BUFFER;
						reproducir_audio(AUDIO_MODO);
						reproducir_audio(AUDIO_BUFFER);
						lcdprintf("Modo","Buffer");
					}
				}
			} else {		//Si se recibe papel:
				if (SENSOR_PAPEL_DEBOUNCED) {
					control_de_papel(0,1,0);	//ingresar
				}
			}
		}
	}

	return Papel;
}
