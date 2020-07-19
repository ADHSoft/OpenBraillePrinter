/*
 * Open Braille Printer (C) 2017-2019 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

#include "DR_Inicializacion.h"
#include "DR_pinsel.h"
#include "DR_motores.h"
#include "DR_Infotronic.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
//bobinas motor de cabezal (motorC) , deben conectarse de que con la secuencia A,B,C,D el carro mueva hacia la der.

#define 	MOTORCA 	EXPANSION6
#define 	MOTORCB 	EXPANSION7
#define 	MOTORCC	 	EXPANSION8
#define 	MOTORCD	 	EXPANSION9

//bobinas motor de hoja (motorH) , deben conectarse de que con la secuencia A,B,C,D la hoja entre.

#define		MOTORHA 	EXPANSION12
#define		MOTORHB		EXPANSION13
#define		MOTORHC		EXPANSION14
#define		MOTORHD		EXPANSION15

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

__RW motorpap_t  MotorCabezal, MotorHoja;
__RW motorpap_t* MotoresPaP[2]; 	// [0: MotorCabezal / carro ; 1: MotorHoja]
__RW bool_t      hw_motores_inicializado = FALSE;
__RW motor_solenoide_t Martillo;

/***********************************************************************************************************************************
 *** VARIABLES GLOBLES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
void motor_pap_map_port( __RW motorpap_t* motor, uint8_t PA1, uint8_t PA2, uint8_t PB1, uint8_t PB2, uint8_t PC1, uint8_t PC2, uint8_t PD1, uint8_t PD2 );
void motorpap_step     ( __RW motorpap_t* motor , int8_t actualizar_pos );
void prender_bobina_correspondiente( __RW motorpap_t* motor );
void apagar_motor      ( __RW motorpap_t* motor );
void apagar_bobinas( __RW motorpap_t* motor );

/***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/******************************************************************************************************************
 * @fn void motor_pap_map_port(motorpap_t *motor,uint8_t PA1,uint8_t PA2,uint8_t PB1,uint8_t PB2,uint8_t PC1,uint8_t PC2,uint8_t PD1,uint8_t PD2 )
 * @brief   Mapeo de los puertos del motor paso a paso
 * @param:  Puerto y bit de PA, PB, PC y PD
 * @return: void
 *****************************************************************************************************************/
void motor_pap_map_port( __RW motorpap_t *motor, uint8_t PA1, uint8_t PA2, uint8_t PB1, uint8_t PB2, uint8_t PC1, uint8_t PC2, uint8_t PD1, uint8_t PD2 )
{
	motor->puertos[0][0] = PA1;	// PA1: puerto de la bobina A
	motor->puertos[0][1] = PA2;	// PA2:    bit de la bobina A
	motor->puertos[1][0] = PB1;	// PB1: puerto de la bobina B
	motor->puertos[1][1] = PB2;	// PA2:    bit de la bobina B
	motor->puertos[2][0] = PC1;
	motor->puertos[2][1] = PC2;
	motor->puertos[3][0] = PD1;
	motor->puertos[3][1] = PD2;
}

/******************************************************************************************************************
 * @fn      void motorpap_step( motorpap_t* motor )
 * @brief   Avanza en un paso al motor.
 * @param:  motor
 * @return: void
 *****************************************************************************************************************/
void motorpap_step( __RW motorpap_t* motor , int8_t actualizar_pos )
{
	motor->estado_corriente    = ENERGIZANDO;
	motor->tiempo_transcurrido = 0;

	//cambiar el estado de la secuencia
	if( motor->pasos_pendientes > 0 )
	{
		motor->estado_bob++;
		motor->pasos_pendientes--;
		if (actualizar_pos) motor->posicion++;
	}
	else
	{
		motor->estado_bob--;
		motor->pasos_pendientes++;
		if (actualizar_pos) motor->posicion--;
	}

	//corregir cuando se va de los limites de la secuencia
	if( motor->estado_bob == 4 )	//overflow
		motor->estado_bob = 0;

	if( motor->estado_bob <0 || motor->estado_bob > 3 )	//underflow
		motor->estado_bob = 3;

	prender_bobina_correspondiente( motor );
}

/******************************************************************************************************************
 * @fn      void prender_bobina_correspondiente( motorpap_t* motor )
 * @brief   Prende las bobinas correspondientes según la secuencia paso a paso.
 * @param:  motor
 * @return: void
 *****************************************************************************************************************/
void prender_bobina_correspondiente( __RW motorpap_t* motor )
{
	apagar_bobinas( motor );	//se ponen las cuatro bobinas a 0 para luego poner en 1 las que correspondan.

	//por las dudas verificar estado invalido
	if( motor->estado_bob < 0 || motor->estado_bob > 3 ) return;

	switch( motor->tipo_drive )
	{
		case FULL_STEP:	//prender bobina = estado
			SetPIN( motor->puertos[(uint8_t)motor->estado_bob][0] , motor->puertos[(uint8_t)motor->estado_bob][1] , ON );
			break;

		case FULL_STEP_POWER:	//secuencia: ab,bc,cd,da
			if( motor->estado_bob == 3) //ultimo estado(especial): prender d y a
			{
				SetPIN( motor->puertos[0][0] , motor->puertos[0][1] , ON );
				SetPIN( motor->puertos[3][0] , motor->puertos[3][1] , ON );
			}
			else	//prender estado y estado+1
			{
				SetPIN( motor->puertos[(uint8_t)motor->estado_bob  ][0] , motor->puertos[(uint8_t)motor->estado_bob  ][1] , ON );
				SetPIN( motor->puertos[(uint8_t)motor->estado_bob+1][0] , motor->puertos[(uint8_t)motor->estado_bob+1][1] , ON );
			};
			break;
		default:
			break;
	}
}

/******************************************************************************************************************
 * @fn      void apagar_motor( motorpap_t* motor )
 * @brief   Apaga el motor seleccionado por medio de la funcion apagar_bobinas( motor)
 * @param:  motor: puntero a estructura motorpap_t
 * @return: void
 *****************************************************************************************************************/
void apagar_motor( __RW motorpap_t* motor )
{
	motor->estado_corriente    = DESUSO;
	motor->tiempo_transcurrido = 0;
	apagar_bobinas( motor );
}

/******************************************************************************************************************
 * @fn      void apagar_bobinas( motorpap_t* motor )
 * @brief   Pone las cuatro bobinas en off
 * @param:  motor
 * @return: void
 *****************************************************************************************************************/
void apagar_bobinas( __RW motorpap_t* motor )
{
	uint8_t i;

	//bobinas a 0
	for( i = 0 ; i < 4 ; i++ )
		SetPIN( motor->puertos[i][0] , motor->puertos[i][1] , OFF );
}


/***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/
/******************************************************************************************************************
 * @fn      void reiniciar_motor(int motor)
 * @brief   Detiene un motor para cuando está descalibrado.
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void reiniciar_motor( uint8_t motor){

	MotoresPaP[motor]->pasos_pendientes=0;
	MotoresPaP[motor]->posicion=0;
}

/******************************************************************************************************************
 * @fn      void motores_inicializacion( void )
 * @brief   Inicializa las estructuras de los motores
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void motores_inicializacion( void )
{
	uint8_t i, j;

	MotoresPaP[0] = &MotorCabezal;
	MotoresPaP[1] = &MotorHoja;

	// Aclaracion: MOTORCA = 1,25 => PA1 = 1 (puerto) , PA2 = 25 (bit)
	motor_pap_map_port( &MotorCabezal , MOTORCA , MOTORCB , MOTORCC , MOTORCD );	// Importante
	MotorCabezal.estado_bob = 0;
	MotorCabezal.tiempo_on  = 3;//2
	MotorCabezal.tiempo_off = 0;
	MotorCabezal.tiempo_transcurrido = 0;
	MotorCabezal.tipo_drive       = FULL_STEP_POWER;
	MotorCabezal.estado_corriente = DESUSO;
	MotorCabezal.posicion=0;

	motor_pap_map_port( &MotorHoja , MOTORHA , MOTORHB , MOTORHC , MOTORHD );	   // Importante
	MotorHoja.estado_bob = 0;
	MotorHoja.tiempo_on  = 2;
	MotorHoja.tiempo_off = 0;
	MotorHoja.tiempo_transcurrido = 0;
	MotorHoja.tipo_drive       = FULL_STEP_POWER;
	MotorHoja.estado_corriente = DESUSO;
	MotorHoja.posicion=0;

	Martillo.estado_corriente = DESUSO;
	Martillo.tiempo_on  = 7;
	Martillo.tiempo_off = 10;
	Martillo.tiempo_transcurrido = 0;

	for( j = 0 ; j < 2 ; j++ )
	{
		for( i = 0 ; i < 4 ; i++ )
		{
			SetPINSEL( MotoresPaP[j]->puertos[i][0] , MotoresPaP[j]->puertos[i][1] , FUNCION_GPIO );
			SetDIR   ( MotoresPaP[j]->puertos[i][0] , MotoresPaP[j]->puertos[i][1] , SALIDA );
			SetPIN   ( MotoresPaP[j]->puertos[i][0] , MotoresPaP[j]->puertos[i][1] , OFF );
		}
	}

	SetPINSEL( MARTILLO , FUNCION_GPIO );
	SetDIR   ( MARTILLO , SALIDA );
	SetPIN   ( MARTILLO , OFF );

	hw_motores_inicializado = TRUE;
}

/******************************************************************************************************************
 * @fn      void actualizar_motores( void )
 * @brief   Actualiza el estado de los motores
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void actualizar_motores( void ) 	//se debe llamar en cada systick
{
	uint8_t i = 0;

	if( hw_motores_inicializado != TRUE ) return;	//comprobar que el driver se haya inicializado

	switch( Martillo.estado_corriente )	//Prender o apagar martillo
	{
		case DESUSO:
			SetPIN( MARTILLO , OFF );
			break;

		case ENERGIZANDO:
			SetPIN( MARTILLO , ON ) ;
			if( Martillo.tiempo_transcurrido >= Martillo.tiempo_on )
			{
				SetPIN( MARTILLO , OFF );
				Martillo.estado_corriente    = DESENERGIZANDO;
				Martillo.tiempo_transcurrido = 0;
			}
			break;

		default:
		case DESENERGIZANDO:
			SetPIN( MARTILLO , OFF );
			if( Martillo.tiempo_transcurrido >= Martillo.tiempo_off )
			{
				Martillo.estado_corriente    = DESUSO;
				Martillo.tiempo_transcurrido = 0;
			}
			break;
	}

	if ( Martillo.tiempo_transcurrido <100 )	// prevencion overflow, total no importa seguir contando
		Martillo.tiempo_transcurrido++;			//este contador se encarga de prender o apagar el tiempo necesario

	for( i = 0 ; i < 2 ; i++ ) 	//para los dos motores
	{
		switch( MotoresPaP[i]->estado_corriente )	//prender el motor (como corresponda) o dejarlo apagarlo.
		{
			case ENERGIZANDO:
				if( MotoresPaP[i]->tiempo_transcurrido == 0 )
					if (Martillo.estado_corriente!=ENERGIZANDO) {
						motorpap_step( MotoresPaP[i] , !i );
					} else {
						MotoresPaP[i]->tiempo_transcurrido--;	//pausar motores al accionar martillo
					}
				else
				{
					if( MotoresPaP[i]->tiempo_on <= MotoresPaP[i]->tiempo_transcurrido ) 	// estuvo prendido el tiempo suficiente
					{																		// tiempo_transcurrido >= tiempo_on
						if( MotoresPaP[i]->tiempo_off != 0 )  	//pasar a enfriar
						{
							apagar_bobinas( MotoresPaP[i] );
							MotoresPaP[i]->estado_corriente    = DESENERGIZANDO;
							MotoresPaP[i]->tiempo_transcurrido = 0;
						}
						else
						{
							if( MotoresPaP[i]->pasos_pendientes != 0 )  //pasar al siguiente paso sin enfriar
								motorpap_step( MotoresPaP[i] , !i );
							else
								apagar_motor ( MotoresPaP[i] );
						}
					}
				}
				break;

			case DESENERGIZANDO:
				if( MotorCabezal.tiempo_off <= MotorCabezal.tiempo_transcurrido ) 	// si ya estuvo apagado lo necesario
				{																	// tiempo_transcurrido >= tiempo_off
					if( MotoresPaP[i]->pasos_pendientes == 0 )
						apagar_motor ( MotoresPaP[i] );
					else 	//dar otro paso
						motorpap_step( MotoresPaP[i] , !i );
				}
				break;

			case DESUSO:
				if( MotoresPaP[i]->pasos_pendientes != 0 )
					motorpap_step( MotoresPaP[i] , !i );
				else
					apagar_bobinas( MotoresPaP[i] );

			default:
				break;
		}

		if (MotoresPaP[i]->tiempo_transcurrido <100 )	// prevencion overflow, total no importa seguir contando
			MotoresPaP[i]->tiempo_transcurrido++;	//este contador se encarga de prender o apagar el tiempo necesario
	}
}

