/*
 * Open Braille Printer (C) 2017-2019 Alejandro Herme.
 * See LICENSE.txt for more details. Contact : alejandrodherme@gmail.com
*/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "DR_motores.h"
#include "PR_motores.h"
#include "DR_gpio.h"
#include "PR_debounce.h"
#include "PR_lcd.h"

#ifndef GOLD_VERSION
#include "free/gold_obp_placeholders.h"
#else
#include "./gold/PR_audio.h"
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
bool_t cabezal_calibrado=0;

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

bool_t calibracion_cabezal(){

	static bool_t no_anda_el_sensor_de_fin=FALSE;

	if (no_anda_el_sensor_de_fin) {		//medida de seguridad, si se activa hay que resetear el aparato.
		return FALSE;
	}

	if ( motores_parados() == TRUE ) {

 		if (!cabezal_calibrado) {//calibracion
			if ( SENSOR_FIN_CARRO_DEBOUNCED ==0 ) {
				mover_motor(MOTOR_CARRO,-1);
				if (MotorCabezal.posicion<-400) {
					lcdprintf("Error sensor FC.","");
					reproducir_audio(AUDIO_ERROR_CRITICO);
					no_anda_el_sensor_de_fin=TRUE;
				}
			} else {
				cabezal_calibrado=TRUE;	//tocó el sensor. setear posicion 0.
				MotorCabezal.posicion=0;
			}

		} else {


		}

	} else {	//prevenciones para si el sensor de f.d.c. falla

		//el sensor de fin de carro está dando contacto cuando no deberia, recalibrar.
		if ( cabezal_calibrado==TRUE && MotorCabezal.posicion>20 && SENSOR_FIN_CARRO_DEBOUNCED == 1 ) {
			lcdprintf("Error sensor FC.","");
			reproducir_audio(AUDIO_ERROR_CRITICO);
			no_anda_el_sensor_de_fin=TRUE;
			cabezal_calibrado=FALSE;
		}

		if (cabezal_calibrado==TRUE && MotorCabezal.posicion<-10 ) {
			lcdprintf("Error sensor FC.","/ bug ");
			reproducir_audio(AUDIO_ERROR_CRITICO);
			no_anda_el_sensor_de_fin=TRUE;
			cabezal_calibrado=FALSE;
		}
	}

	return cabezal_calibrado;
}

bool_t motores_parados(){

	if ( motor_listo(0) == FALSE || motor_listo(1) == FALSE || Martillo.estado_corriente != DESUSO )
		return FALSE;

	return TRUE;

}

/******************************************************************************************************************
 * @fn      bool_t posicionar_motor( int8_t pos_x , int8_t pos_y , bool_t martillar_al_estar_en_posicion )
 * @brief   Mueve el cabezal a x,y. Permite martillar cuando se terminó de posicionar.
 * @param:  x,y,martillar_al_posicionarse
 * @return: Si los motores están en posicion y terminaron de moverse, devuelve TRUE. Sino, FALSE.
 *****************************************************************************************************************/
bool_t posicionar_motor_y_martillar( int16_t pos_x , bool_t martillar_al_estar_en_posicion ) {

	static bool_t martillada_pendiente=FALSE;

	if (motores_parados()==TRUE ) {
		if ( pos_x==MotorCabezal.posicion ) {
			if (martillada_pendiente==TRUE) {
				martillar();
				martillada_pendiente=FALSE;
			}
			return TRUE;
		} else {
			mover_motor(MOTOR_CARRO, pos_x-MotorCabezal.posicion );
			return FALSE;
		}
	} else {	//en movimiento/martillando
		//if (motor_listo(MOTOR_CARRO)==TRUE) mover_motor(MOTOR_CARRO, pos_x-MotorCabezal.posicion );
		if ( martillar_al_estar_en_posicion==TRUE && Martillo.estado_corriente==DESUSO ) martillada_pendiente=TRUE;
		return FALSE;
	}

}

/******************************************************************************************************************
 * @fn      void mover_motor( uint8_t motor , int16_t pasos )
 * @brief   Actualiza la cantidad de pasos pendientes del motor seleccionado
 * @param:  #motor: [0: MotorCabezal - 1: MotorHoja]
 * @param:  pasos: Cantidad de pasos que se deben agregar a pasos_pendientes
 * @return: void
 *****************************************************************************************************************/
void mover_motor( uint8_t motor , int16_t pasos )
{
	MotoresPaP[ motor ]->pasos_pendientes += pasos;
}

/******************************************************************************************************************
 * @fn      bool_t motor_listo( uint8_t motor )
 * @brief   Indica si el motor esta en DESUSO (apagado y sin pasos pendientes por moverse)
 * @param:  motor: [0: MotorCabezal - 1: MotorHoja]
 * @return: Estado del motor (TRUE-FALSE)
 *****************************************************************************************************************/
bool_t motor_listo( uint8_t motor )
{
	if( ( MotoresPaP[ motor ] )->estado_corriente == DESUSO &&  ( MotoresPaP[ motor ] )->pasos_pendientes == 0 )
		return TRUE;
	return FALSE;
}

/******************************************************************************************************************
 * @fn      void parada_emergencia_motores( void )
 * @brief   Apaga todos los motores
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void parada_emergencia_motores( void )
{
	uint8_t i, j;

	//apagar bobinas
	SetPIN( MARTILLO , OFF );

	if( hw_motores_inicializado == FALSE )
		return;

	for( j = 0 ; j < 2 ; j++ )
		for( i = 0 ; i < 4 ; i++ )
			SetPIN( MotoresPaP[j]->puertos[i][0] , MotoresPaP[j]->puertos[i][1] , OFF );
}

/******************************************************************************************************************
 * @fn      void martillar( void )
 * @brief	Cambia el estado a ENERGIZANDO, para así, martillar en la proxima actualizacion de los motores.
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void martillar( void )
{
	Martillo.estado_corriente    = ENERGIZANDO;
	Martillo.tiempo_transcurrido = 0;
}
