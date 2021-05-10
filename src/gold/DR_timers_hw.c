/*******************************************************************************************************************************//**
 *
 * @file		DR_gpio.c
 * @brief		Descripcion del modulo
 * @date		1 de abr. de 2016
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "DR_timers_hw.h"
#include "./../DR_gpio.h"
#include "./../DR_pinsel.h"

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
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************
  \ fn      void isr_ini_TMR0 (void)
  \ brief:  Aplicación para inicializar la ISR de TIMER 0
  \param [in]  void
  \return      void
***********************************************************************************/
/*void isr_ini_TMR0( void )
{
	PCONP    |=  ( 1 << 1 );  // Encender Timer0  por Default amanece encendido Pag 63
	PCLKSEL0 &= ~( 3 << 2 );  // Clock for timer = CCLK/4 Selecciono clock Pag 56
  //PCLKSEL0 |=  ( 1 << 2 );  // Clock for timer = CCLK Selecciono clock Pag 56

	T0MR0 = MATCH0_0;	// Configuro el tiempo del MATCH0
	T0MR1 = MATCH0_1;	// Configuro el tiempo del MATCH1

	T0MCR |= 1; 	// Configuro el MATCH0 para que interrumpa solamente Activo el bit 0: MROI  Pag 496
	 	            // Por defecto los bits 1 MR0R(RESET) y MR0S(STOP) estan en cero con lo que entonces solo
	 	            // interrumpirá

	T0MCR |= ( 3 << 3 ); // Configuro el MATCH1 para que interrumpa y resetee el timer poniendo un 1 en
	 	                     // los bits 3 (MR1I) y 4(MRIR) , el bit 5 (MR1S) amanece en cero

	T0TCR &= ~(1);	 	   // Apago  el temporizador, poniendo un cero en el bit 0 Pag 494
	T0TCR |= ( 1 << 1 );   // Reseteo el temporizador, prendiendo el bit 1

	T0TCR &= ~( 1 << 1 );  // Apago el bit 1 de RESET
	T0TCR |= 1;            // Enciendo el temporizador poniendo el bit 0 de ENABLE en 1

	ISE_TIMER0; 	       // Habilito interrupcion del Timer0 en el vector de interrupciones.
}
*/
/*******************************************************************************************************
   \ fn     void ini_TMR1( void )
   \ brief: Aplicación para inicializar el TIMER 1, no se lo usará por INTERRUPCION
   \        se trabajará directamente sobre la salida:
   \                P1[22] MAT1.0 EXP8 de la placa base (CN4-6)
   \        Al excitar directamente el PIN de salida y no pasar por el micro
   \        aún estando el micro en STOP (durante el DEBUGGING) las salidas oscilan normalmente
   \param [in]  void
   \return      void
********************************************************************************************************/
/*void ini_TMR1( void )
{
	 PCONP    |= ( 1 << 2 ); 	// Habilitar Timer 1
	 PCLKSEL0 |= ( 1 << 4 );	// Clock for timer = CCLK Selecciono clock Pag 56

	 SetPINSEL( PORT1 , 22 , PINSEL_FUNC3 ); // selecciono al P1[22]como MAT1.0
	 SetDIR   ( PORT1 , 22 , SALIDA );
	 SetMODE  ( PORT1 , 22 , PULLUP );

	 T1MR0 = MATCH1_0;	 	// Configuro el tiempo del match 0

	 T1MCR |= ( 1 << 1); 	// Configuro el match 0 para que RESETE activando el bit 1: MR0R (del TIMER 1)  Pag 496
					     	// NO SE GENERARA UNA INTERRUPCION
	 EMCO_TOGGLE; 		 	// Configurado como TOGGLE el pin MAT1.0
	 T1TCR &= ~(1);	   		// Apago  el temporizador, poniendo un cero en el bit 0
	 T1TCR |= ( 1 << 1 );   // Resteo el temporizador, prendiendo el bit 1

	 T1TCR &= ~( 1 << 1 );  // Apago el bit 1 de RESET
	 T1TCR |= 1;            // Enciendo el temporizador poniendo el bit 0 de ENABLE en 1
}
*/
