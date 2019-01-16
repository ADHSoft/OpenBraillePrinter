/*
 * Open Braille Printer (C) 2017-2019 Alejandro Herme.
 * See LICENSE.txt for more details. Contact : alejandrodherme@gmail.com
*/


#ifndef GOLD_VERSION
#include "free/gold_obp_placeholders.h"
#else
#include "./gold/PR_audio.h"
#include "./gold/PR_UART.h"
#endif

#include "DR_Inicializacion.h"
#include "DR_tipos.h"

#include "PR_motores.h"
#include "PR_Timers.h"
#include "PR_lcd.h"
#include "AP_Funciones.h"
#include "PR_Teclado_PS2.h"
#include "PR_debounce.h"

#include "AP_Buffer.h"
#include "AP_mde_Impresion.h"


/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
#define EV_RESET_PC 21

#define BUFFER		0
#define INMEDIATO	1
#define PC			2


/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
int8_t Modo=BUFFER;
bool_t Papel=FALSE,Sonido=TRUE,Pausa=FALSE;

 /***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
void mde_impresion( bool_t );
void responder_PC( bool_t );
void reset_responder_PC ();
void responder_al_usuario();

 /***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/

int main(void)
{

	Inicializacion();

    lcdprintf( "O.B.P. Driver","Build Jan 2019");
    reproducir_audio(AUDIO_BIENVENIDO);

    delay_s(2); //2 sec delay

    lcdprintf( "(C) AlejandroD","Herme@gmail.com");
    delay_s(2);

	while(1) {

		//Controlar cambio en sensor de papel, para detener la impresion
		Papel=control_de_papel(0,0,&Modo);

		//Imprimir
		if ( Modo==INMEDIATO || Modo==PC ) buffer_poner_en_cola_impresion();
		if ( Papel && !Pausa && calibracion_cabezal()==TRUE )	mde_impresion(FALSE);

		//salir de modo PC
		if(Modo==PC && buffer_get_cola_impresion()==0 && buffer_get_renglon_actual()==0 ) {
			reproducir_audio(AUDIO_LISTO_PARA_IMPRIMIR);
			lcdprintf("Archivo Impreso","desde PC");
			Modo=BUFFER;
		}

		//Ejecutar eventos de timers
		TimerEvent();

		//calibrar cabezal (ajustar pos. 0 segun fin de carro)
		calibracion_cabezal();

		//responder al teclado y a PC
		responder_al_usuario();
		responder_PC(FALSE);

	}

}

/******************************************************************************************************************
 * @fn		void responder_PC( void )
 * @brief   Ejecuta comandos o guarda caracteres a imprimir según lo que se reciba desde PC.
 * @details  void
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void responder_PC( bool_t reset ){

	#define COMANDO_MOVER 0xFE
	#define EXCESO_MOVER 10
	#define COMANDO_FUNCION 0xFF
	#define FUNCION_EXPULSAR 0xFD
	#define FUNCION_PREGUNTAR_LISTO 0xFB

	int16_t recepcion;	//caracter comun imprimible: un byte distinto de FFh y FEh.
						//Comando especial: dos bytes, el primero FFh o FEh segun comando.
	static int16_t comando_extendido=0; 	//para guardar el primer byte (cuando es comando).

	if (reset) {
		comando_extendido=0;
		return;
	}

	if ( (recepcion= UART0_PopRx())!=-1 ) {

		if (comando_extendido!=0) {	//si antes se recibio un byte de comando, interpretar comando:

			switch (comando_extendido) {
				case COMANDO_MOVER:	//el segundo byte recibido son los pasos con exceso 10
					mover_motor(MOTOR_HOJA,(recepcion-10)*4);
					break;

				default:
				case COMANDO_FUNCION:
					switch(recepcion) {	//segundo byte:
						case FUNCION_EXPULSAR:
							control_de_papel(1,0,0);//Expulsar_hoja
						break;
						case FUNCION_PREGUNTAR_LISTO:
							if ( Papel && buffer_get_cola_impresion()==0 && buffer_get_editable()==0 ) {
								UART0_PushTx('Y');
							} else {
								UART0_PushTx('N');
							}
						break;
					}
					break;
			}
			comando_extendido=0;
			TimerStop(EV_RESET_PC);
		} else {
			//si no se habia recibido un codigo de comando extendido antes:

			if ( recepcion==COMANDO_FUNCION || recepcion==COMANDO_MOVER ) {
				comando_extendido=recepcion;	//guardar este primer byte para despues
				TimerStart(EV_RESET_PC,5,DEC,reset_responder_PC);	//time out
			} else {
				//es un caracter imprimible. Por las dudas ignorar los '\r' que podrian llegar. Ya que salto de linea es \n unicamente.

				if (Modo!=PC) {
					Modo=PC;
					reproducir_audio(AUDIO_MODO);
					reproducir_audio(AUDIO_PC);
					mde_impresion(TRUE);	//reiniciar MDE Impresion
					lcdprintf("Conectado a PC.","ESC: Cancelar y Salir");

				}

				if (recepcion!='\r') buffer_agregar_caracter(recepcion);
			}

		}

	}
}

/******************************************************************************************************************
 * @fn		void responder_al_usuario( void )
 * @brief   Ejecuta comandos o guarda caracteres a imprimir según lo que se reciba desde teclado.
 * @details  void
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void responder_al_usuario(){

	//Se llama a interpretar_buffer_PS2. Si devuelve una acción a realizar es que
	//setea la variable Tecla_Letra o Tecla_Funcion, que luego se evalúa que ejecutar en los switch.

	interpretar_buffer_PS2(0);

	if (Tecla_Letra!=0) {
		if (Sonido) reproducir_audio(AUDIO_SONIDO_TECLA);
		if (Modo==PC) {
			lcdprintf("Conectado a PC.","ESC: Cancelar y Salir");
		} else {
			if ( buffer_agregar_caracter( Tecla_Letra ) != 0 ) {
				detener_audio();	//no hay espacio en buffer
				lcdprintf("Espacio Agotado","");
				reproducir_audio(AUDIO_NO_HAY_MAS_MEMORIA_PARA_ESCRIBIR);
			} else {
				lcdprintf(MOSTRAR_BUFFER_EDITABLE);
			}
		}
		Tecla_Letra=0;
		Tecla_Funcion=0;
	}
	if (Tecla_Funcion!=0){
		if (Sonido) reproducir_audio(AUDIO_SONIDO_TECLA);
		switch(Tecla_Funcion) {
			case F1: if(Sonido==FALSE) {
					reproducir_audio(AUDIO_SONIDO_ACTIVADO);
					lcdprintf("Sonido de tecla","Activado");
				} else {
					lcdprintf("Sonido de tecla","Desactivado");
				}

				Sonido=!Sonido;
				break;
			case INICIO: if (Papel==TRUE) {
				buffer_poner_en_cola_impresion();
				lcdprintf("Enviado a cola","de impresion");
			} else {
				buffer_cancelar_cola_impresion();
				reproducir_audio(AUDIO_NO_HAY_HOJA);
				lcdprintf("No hay hoja.","");
			}
			break;
			case FIN:
				control_de_papel(1,0,0);//Expulsar_hoja
				break;
			case PG_DN:	//ajustar pos. vertical hoja
				if (motor_listo(MOTOR_HOJA) == TRUE ) {
					mover_motor(MOTOR_HOJA,30);
				}
				break;
			case BACKSPACE: buffer_borrar_char();
				lcdprintf(MOSTRAR_BUFFER_EDITABLE);
				break;

			case ESC: if (Modo==PC) {
					reproducir_audio(AUDIO_MODO);
					reproducir_audio(AUDIO_BUFFER);
					lcdprintf("Modo Buffer","");
					Modo=BUFFER;
				} else {
					if (buffer_get_cola_impresion()!=0) {
						buffer_cancelar_cola_impresion();
						lcdprintf("Cola de Impresion","Cancelada");
					} else {
						buffer_borrar_editable();
						lcdprintf("Texto Eliminado","");
						reproducir_audio(AUDIO_LISTO_PARA_IMPRIMIR);
					}
				}
				break;
			case F2: switch(Modo) {
				case INMEDIATO:
					reproducir_audio(AUDIO_MODO);
					reproducir_audio(AUDIO_BUFFER);
					lcdprintf("Modo Buffer","");
					Modo=BUFFER;
					break;
				case BUFFER:
					reproducir_audio(AUDIO_MODO);
					reproducir_audio(AUDIO_INMEDIATO);
					lcdprintf("Modo Inmediato","");
					Modo=INMEDIATO;
					break;
				case PC :
					lcdprintf("Conectado a PC.","ESC: Cancelar y Salir");
				break;
			}
			break;
			case F5: Pausa=!Pausa;
				if (Pausa) lcdprintf("Pausa Activada","");
				if (!Pausa) lcdprintf("Pausa Desactivada","");
			break;
			case F7:	//teclado normal
				ch_kbd_mode(KBD_FULL_KEYBOARD);
				lcdprintf("Teclado Normal.","");
				break;
			case F8:	//teclado clasico 6B
				ch_kbd_mode(KBD_CLASSIC_SIXBUTTONS);
				lcdprintf("Teclado de seis","botones.");
				break;
			default:
				detener_audio();	//no reproducir el sonido de tecla
				break;
		}
		Tecla_Letra=0;
		Tecla_Funcion=0;
	}

}

/******************************************************************************************************************
 * @fn		void reset_responder_PC( void )
 * @brief   time out para el recibo de un comando de 2 bytes por PC
 * @details  void
 * @param:  void
 * @return: void
 *****************************************************************************************************************/

void reset_responder_PC (){
	responder_PC(TRUE);
}

