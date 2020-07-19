/*
 * Open Braille Printer (C) 2017-2019 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "PR_Timers.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
#define 	DECIMAS			40
#define 	SEGUNDOS		10
#define 	MINUTOS			60

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
static volatile uint8_t  TmrBase     [ N_TIMERS ] ; // Base de tiempo del evento
static volatile uint32_t TmrRun      [ N_TIMERS ] ; // Se lo carga con el valor del tiempo del evento
static volatile uint8_t  TmrEvents   [ N_TIMERS ] ; // Flag del timer para el evento, el cual, avisa cuando vence el timer
static volatile uint8_t  TmrStandBy  [ N_TIMERS ] ;
static void           ( *TmrHandlers [ N_TIMERS ] )( void ) ;

/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/

/**********************************************************************************************************************************
	\fn      void TimerStart( uint8_t event , uint32_t time ,  uint8_t base , void (*handler)(void) )
	\brief   Inicia un timer 	\details Inicia el timer \a event al transcurrir el tiempo especificado por \a time se llama a la funcion apuntada por \a handler
 	\param   [in]   event: Numero de evento entre 0 y 29
 	\param   [in]    time: Tiempo del evento. Dependiente de la base de tiempos
 	\param   [in]    base: base de tiempo ( DEC, SEG, MIN )
 	\param   [in] handler: Callback del evento
	\return  void
************************************************************************************************************************************/
void TimerStart( uint8_t event , uint32_t time , uint8_t base , TmrHandler handler )
{
	switch ( base )
	{
		case DEC:
			time *= DECIMAS;
			break;
		case SEG:
			time *= ( SEGUNDOS * DECIMAS );
			break;
		case MIN:
			time *= ( MINUTOS * SEGUNDOS * DECIMAS );
			break;
	}

	TmrBase [ event ] = base ;  // Lo carga con la base de tiempo del evento dado

	if( time != 0 )	// El tiempo no es 0, lo cargo
	{
		TmrRun    [ event ] = time ;  // Carga el timer del evento dado
		TmrEvents [ event ] = 0 ;     // Flag del timer para el evento dado en 0
	}
	else				// El tiempo es 0, el timer vence automáticamente
	{
		TmrRun    [ event ] = 0 ;  	  // Lo carga con el valor cero
		TmrEvents [ event ] = 1 ;	  // Pone en 1 el flag, avisa que termino
	}

	TmrHandlers [ event ] = handler;
}

/**********************************************************************************************************************************
	\fn void TimerSet( uint8_t event , timer_t time )
	\brief   Inicia un timer
 	\details Reinicia el timer con el valor `time´ (No lo resetea)
 	\param   [in] event: Numero de evento entre 0 y 29
 	\param   [in]  time: Tiempo del evento. Dependiente de la base de tiempos
 	\return  void
************************************************************************************************************************************/
void TimerSet( uint8_t event , uint32_t time )
{
	switch ( TmrBase [ event ] )
	{
		case DEC:
			time *= DECIMAS ;
			break ;
		case SEG:
			time *= ( SEGUNDOS * DECIMAS ) ;
			break ;
		case MIN:
			time *= ( MINUTOS * SEGUNDOS * DECIMAS ) ;
			break ;
	}

	TmrRun [ event ] = time;
}

/**********************************************************************************************************************************
	\fn      TimerGet( uint8_t event )
	\brief   Toma el valor al vuelo del timer en cuestion
 	\details Lee el timer
 	\param   [in] event: Numero de evento entre 0 y 29
 	\return  Valor del timer
************************************************************************************************************************************/
uint32_t TimerGet( uint8_t event )
{
	uint32_t time = TmrRun [ event ];

	switch ( TmrBase [ event ] )
	{
		case DEC:
			time /= DECIMAS;
			break;
		case SEG:
			time /= ( SEGUNDOS * DECIMAS );
			break;
		case MIN:
			time /= ( MINUTOS * SEGUNDOS * DECIMAS );
			break;
	}

	return time;
}

/**********************************************************************************************************************************
	\fn      TimerStandBy ( uint8_t event , uint8_t accion )
	\brief   Detiene/Arranca el timer, NO lo resetea
 	\details Lo pone o lo saca de stand by
 	\param   [in]  event: Numero de evento entre 0 y 29
 	\param   [in] accion: RUN lo arranca, STOP lo pone en stand by
 	\return  Valor del timer
************************************************************************************************************************************/
void TimerStandBy( uint8_t event , uint8_t accion )
{
	TmrStandBy [ event ] = accion ;
}

/**********************************************************************************************************************************
	\fn      void Timer Stop ( uint8_t event )
	\brief   Detiene un timer
 	\details Detiene el timer event
 	\param   [in] event: Numero de evento entre 0 y 29
	\return  void
************************************************************************************************************************************/
void TimerStop( uint8_t event )
{
	TmrRun      [ event ] = 0 ;     // Carga el timer con el valor 0
	TmrEvents   [ event ] = 0 ;		// Flag en 0
	TmrHandlers [ event ] = NULL ;	// No selecciona ninguna tarea
	TmrBase     [ event ] = 0 ;		// Pone la base de tiempo en 0
	TmrStandBy  [ event ] = RUN ;	// No esta en stand by, sino que lo detiene
}

/**********************************************************************************************************************************
	\fn      void TimerClose ( void )
	\brief   Detiene todos los timers
 	\details Detiene todos los timers
	\return  void
************************************************************************************************************************************/
void TimerClose( void )
{
	uint32_t i ;

	for ( i = 0 ; i < N_TIMERS ; i++ )
		TimerStop ( i ) ;
}

/**********************************************************************************************************************************
	\fn      void TimerCheck ( void )
	\brief   Decremento periodico de los contadores
 	\details Decrementa los contadores de los timers en ejecucion. Debe ser llamada periodicamente con la base de tiempos
	\return  void
************************************************************************************************************************************/
void TimerCheck( void )
{
	uint32_t i ;

	for ( i = 0 ; i < N_TIMERS ; i++ )
	{
		if( TmrRun [ i ] )                 // Si esta cargado
		{
			if ( !TmrStandBy [ i ] )       // Si no esta en stand by
			{
				TmrRun [ i ]-- ;		   // Decrementa el contador
				if(!TmrRun [ i ] )  	   // Si llego a 0
					TmrEvents [ i ] = 1 ;  // Flag en 1, dispara el evento
			}
		}
	}
}

/**********************************************************************************************************************************
	\fn      void TimerEvent ( void )
	\brief   Chequeo de timers vencidos
 	\details Llama a los callbacks de los timers vencidos. Debe llamarse desde el lazo principal del programa
	\return  void
************************************************************************************************************************************/
void TimerEvent( void )
{
	uint8_t i;

	for ( i = 0 ; i < N_TIMERS ; i++ )
	{
		if( TmrEvents[ i ] )   			  // Si algun timer vencio (Flag en 1)
		{
			if( TmrHandlers [ i ]!=NULL) TmrHandlers [ i ]( )   ;  // Realiza la tarea para dicho evento
			TmrEvents   [ i ] = 0  ;  // Pone en 0 el flag
		}
	}
}
