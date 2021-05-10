/*
 * Open Braille Printer (C) 2017-2021 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "DR_dac.h"
#include "./../DR_Infotronic.h"
#include "./../DR_tipos.h"
#include "./../DR_pinsel.h"
#include "./../DR_gpio.h"
#include "DR_timers_hw.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
#define		DACR 		( * ( ( __RW uint32_t * )  0x4008C000 ) )
#define 	DAC_BIAS	 1	//dac en modo de menor velocidad y consumo

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
__RW uint8_t  dac_inicializado  = 0;
__RW uint8_t  playlist_start    = 0;
__RW uint8_t  playlist_cantidad = 0;
__RW uint32_t muestra_actual    = 0;
__RW audio_t* playlist[ PLAYLIST_SIZE ];

/***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/

/******************************************************************************************************************
 * @fn      void dac_y_timer_init( void )
 * @brief   Configuracion del Timer0 y el DAC
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void dac_y_timer_init( void )
{
	SetPINSEL( SALIDA_AUDIO , PINSEL_FUNC2 );	//pin sel 10

	// inicializo TMR0
	PCONP |= ( 1 << 1 ); 		// Encender Timer0  por Default amanece encendido Pag 63
	PCLKSEL0 &= ~( 3 << 2 );    // Clock for timer = CCLK/4 Selecciono clock Pag 56

	T0MR0 = 2268;	// Configuro el tiempo del MATCH0 , para frecuencia de audio de 11025 Hz aprox

	T0MCR |= 3;	    // Configuro el MATCH0 para que interrumpa, y para que resetee el timer

	// prender TMR0
	T0TCR &= ~(1);	       // Apago  el temporizador, poniendo un cero en el bit 0 Pag 494
	T0TCR |= ( 1 << 1 );   // Resteo el temporizador, prendiendo el bit 1

	T0TCR &= ~( 1 << 1 );  // Apago el bit 1 de RESET
	T0TCR |= 1;            // Enciendo el temporizador poniendo el bit 0 de ENABLE en 1

	ISE_TIMER0; 	// Habilito interrupcion del Timer0 en el vector de interrupciones.

	dac_inicializado = 1;
}


/******************************************************************************************************************
 * @fn      void TIMER0_IRQHandler( void )
 * @brief   Funcion de interrupcion del Timer0
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void TIMER0_IRQHandler( void )
{
	static uint8_t muestra;

	if( dac_inicializado && playlist_cantidad != 0) {

		if( muestra_actual == playlist[ playlist_start ]->Size )
		{
			//terminó de reproducir el audio
			muestra_actual = 0;
			playlist_start++;
			playlist_start %= PLAYLIST_SIZE;
			playlist_cantidad--;
		}
		else
		{
			muestra = playlist[ playlist_start ]->pcm_data[ muestra_actual ];
			DACR    = ( muestra << 8 ) + ( DAC_BIAS << 16 );
			muestra_actual++;

		}

	}

	TIMER0.IR |= 1;		//limpiar flag de interrupcion match0 antes de salir de la funcion
}

