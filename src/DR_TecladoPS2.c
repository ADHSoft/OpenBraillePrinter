/*
 * Open Braille Printer (C) 2017-2019 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "DR_gpio.h"
#include "DR_Infotronic.h"
#include "DR_pinsel.h"
#include "PR_Teclado_PS2.h"
#include "PR_Timers.h"
#include "DR_TecladoPS2.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
// Open collector
#define 	PS2_CLK_INPUT 		EXPANSION16
#define 	PS2_CLK_OUTPUT	 	EXPANSION18	// Salida colector abierto, esta salida esta invertida. dejarla en 0v (1). concide con RX1
#define 	PS2_DATA_INPUT 		EXPANSION17
#define 	PS2_DATA_OUTPUT		EXPANSION19	// Salida colector abierto, esta salida esta invertida. dejarla en 0v (1). concide con TX1

//si se desea cambiar los pines, se deben cambiar un par de funciones de este archivo. Buscar los comentarios en mayuscula.
//el unico tema para no molestar al uart1 en otros proyectos, es que los inputs no usen sus pines.

#define 	PS2DATA 	GetPIN( PS2_DATA_INPUT , ALTO )

/***********************************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 **********************************************************************************************************************************/
__RW uint8_t BufferPS2RX[ TAMANO_RX ];
__RW uint8_t cantidadRX   = 0;
__RW uint8_t IndexStartRX = 0;

__RW uint8_t BufferPS2TX[ TAMANO_TX ];
__RW uint8_t cantidadTX   = 0;
__RW uint8_t IndexStartTX = 0;
/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
void Interrupcion_Externa_Inicializacion( void );
void Teclado_PS2_Timeout( void );

 /***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/******************************************************************************************************************
 * @fn		void Interrupcion_Externa_Inicializacion( void )
 * @brief	Inicializacion de las interrupciones externas
 * @details  Habilita EINT3 y configura el puerto P0.19 como GPIO Interrupt
 * @param:  void
 * @return: void
 *****************************************************************************************************************/

void Interrupcion_Externa_Inicializacion( void )
{
	SetPINSEL( PS2_CLK_INPUT , PINSEL_GPIO );
	SetDIR   ( PS2_CLK_INPUT , ENTRADA );
	SetMODE  ( PS2_CLK_INPUT , PULLUP );

	ISE_EINT3;		// Habilita la EINT3, receptor de interrupcion por gpio generica

	//EDITAR AL CAMBIAR PIN TECLADO
	*IO2IntEnF = ( 1 << 8 );	// P2.8 -> int ext flanco negativo y ninguna otra interrupcion mas en el puerto.
}

/******************************************************************************************************************
 * @fn		void Teclado_PS2_Timeout( void )
 * @brief
 * @details
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void Teclado_PS2_Timeout( void )
{
	interpretar_buffer_PS2( TRUE );		// Resetear esta MdE
	SetPIN( RGB_B , OFF );
}

 /***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/

/******************************************************************************************************************
 * @fn		void TecladoPS2_Inicializacion( void )
 * @brief   Inicializacion del teclado PS2
 * @details  Configura los puertos de entrada y salida que usará el teclado PS2
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void TecladoPS2_Inicializacion( void )
{
	SetPINSEL( PS2_DATA_INPUT , PINSEL_GPIO );
	SetDIR   ( PS2_DATA_INPUT , ENTRADA );
	SetMODE  ( PS2_DATA_INPUT , PULLUP  );

	SetPINSEL( PS2_DATA_OUTPUT , PINSEL_GPIO );
	SetDIR   ( PS2_DATA_OUTPUT , SALIDA );
	SetMODE  ( PS2_DATA_OUTPUT , NONE   );
	SetPIN   ( PS2_DATA_OUTPUT , OFF    );	// Para no afectar lectura

	SetPINSEL( PS2_CLK_OUTPUT , PINSEL_GPIO );
	SetDIR   ( PS2_CLK_OUTPUT , SALIDA );
	SetMODE  ( PS2_CLK_OUTPUT , NONE   );
	SetPIN   ( PS2_CLK_OUTPUT , OFF    );	// Para no afectar lectura

	Interrupcion_Externa_Inicializacion();
}


/******************************************************************************************************************
 * @fn		void EINT3_IRQHandler( void )
 * @brief   Rutina de Servicio de Interrupcion (ISR)
 * @details  extint3 es la interrupcion para puerto 0 o 2 de gpio generica
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void EINT3_IRQHandler( void )
{
	static uint8_t bit = 0, paridad = 0, dato = 0;
	static bool_t  mensaje_erroneo = FALSE;

	SetPIN( RGB_B , ON );

	if( bit == 0 || TimerGet(10) == 0 )	// Bit de start
	{
		if( TimerGet(10) == 0 ) bit = 0;
		mensaje_erroneo = FALSE;
		paridad = 0;
		dato    = 0;
		if( PS2DATA == ALTO ) mensaje_erroneo = TRUE;	// Bit de start debe ser 0
		TimerStart( 10 , 4 , DEC, Teclado_PS2_Timeout );	// Prender timer para resetear por timeout=20ms
	}

	if( bit >= 1 && bit <= 8 )  // Bits de datos
	{
		dato |= ( PS2DATA << (bit-1) );
		if( PS2DATA == 1 )
			paridad++;		// Cantidad de bits en uno
	}

	if( bit == 9 )
	{	// Insertar codigo para comprobar Paridad impar
	}

	if( bit == 10 )   // Stop
	{
		if( PS2DATA == BAJO ) mensaje_erroneo = TRUE;	// El bit de stop debe ser 1
		TimerStop(10);

		if( mensaje_erroneo == FALSE )	// Si no hubo error en el mensaje
		{
			if( cantidadRX < TAMANO_RX )
			{
				BufferPS2RX[ IndexStartRX ] = dato;	// Guardo el dato en el buffer
				cantidadRX++;
			}
			else
				interpretar_buffer_PS2( TRUE );    // Resetear MdE
		}
		bit = 0;
		SetPIN( RGB_B , OFF );
	}
	else
		bit++;

	//EDITAR AL CAMBIAR PIN TECLADO
	*IO2IntClr = ( 1 << 8 );	// Borrar flag int. ext. P2.8
}
