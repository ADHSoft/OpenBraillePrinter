/*
 * Open Braille Printer (C) 2017-2019 Alejandro Herme.
 * See LICENSE.txt for more details. Contact : alejandrodherme@gmail.com
*/


/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "DR_motores.h"
#include "DR_Infotronic.h"
#include "DR_Inicializacion.h"
#include "DR_pinsel.h"
#include "DR_lcd.h"
#include "DR_TecladoPS2.h"

#ifndef GOLD_VERSION
#include "free/gold_obp_placeholders.h"
#else
#include "./gold/DR_dac.h"
#include "./gold/DR_UART.h"
#endif

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/

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

/***********************************************************************************************************************************
 *** VARIABLES GLOBLES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/******************************************************************************************************************
 * @fn      void Ini_Infotronic( void )
 * @brief   Inicializa las GPIO de la placa Infotronic
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void Ini_Infotronic( void )
{

	SetPINSEL( SW1 , FUNCION_GPIO );	//teclado infotronic
	SetDIR   ( SW1 , ENTRADA );
	SetPINSEL( SW2 , FUNCION_GPIO );	//teclado infotronic
	SetDIR   ( SW2 , ENTRADA );
	SetPINSEL( SW3 , FUNCION_GPIO );	//teclado infotronic
	SetDIR   ( SW3 , ENTRADA );
	SetPINSEL( SW4 , FUNCION_GPIO );	//teclado infotronic
	SetDIR   ( SW4 , ENTRADA );
	SetPINSEL( SW5 , FUNCION_GPIO );	//teclado infotronic
	SetDIR   ( SW5 , ENTRADA );

	//anti ruidos y luces
	SetPINSEL( BUZZER , FUNCION_GPIO );	//buzzer
	SetDIR   ( BUZZER , SALIDA );
	SetPIN   ( BUZZER , ON );

	SetPINSEL( RGB_R , FUNCION_GPIO );	//R
	SetDIR   ( RGB_R , SALIDA );
	SetPIN   ( RGB_R , OFF );

	SetPINSEL( RGB_G , FUNCION_GPIO );	//G
	SetDIR   ( RGB_G , SALIDA );
	SetPIN   ( RGB_G , OFF );

	SetPINSEL( RGB_B , FUNCION_GPIO );	//B
	SetDIR   ( RGB_B , SALIDA );
	SetPIN   ( RGB_B , OFF );/*
	SetPINSEL( LPC_RGB_R ,FUNCION_GPIO);	//lpc R
	SetDIR( LPC_RGB_R , SALIDA);
	SetPIN(LPC_RGB_R,ON);
	SetPINSEL(LPC_RGB_G,FUNCION_GPIO);	//lpc G
	SetDIR( LPC_RGB_G , SALIDA);
	SetPIN( LPC_RGB_G,ON);
	SetPINSEL( LPC_RGB_B,FUNCION_GPIO);	//lpc B
	SetDIR( LPC_RGB_B , SALIDA);
	SetPIN( LPC_RGB_B,ON);*/

	SetPINSEL( RELAY1 , FUNCION_GPIO );
	SetPINSEL( RELAY2 , FUNCION_GPIO );
	SetPINSEL( RELAY3 , FUNCION_GPIO );
	SetPINSEL( RELAY4 , FUNCION_GPIO );

	SetDIR( RELAY1 , SALIDA );
	SetDIR( RELAY2 , SALIDA );
	SetDIR( RELAY3 , SALIDA );
	SetDIR( RELAY4 , SALIDA );

	SetPIN( RELAY1 , OFF );
	SetPIN( RELAY2 , OFF );
}

/***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/

/******************************************************************************************************************
 * @fn      void Inicializacion( void )
 * @brief   Inicializacion general. Esta funcion llama a la mayoria de las funciones de inicializacion
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void Inicializacion( void )
{
	//!< LPC1769
	InitPLL();		//!< Configuro en primer lugar el PLL:Estabiliza el oscilador interno del micro en 100MHz

	Ini_Infotronic();

	lcd_inicializacion();

	motores_inicializacion();

	dac_y_timer_init();

	SysTick_Inicializacion();

	TecladoPS2_Inicializacion();

	InitUART0();

}
