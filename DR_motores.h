/*
 * Open Braille Printer (C) 2017-2019 Alejandro Herme.
 * See LICENSE.txt for more details. Contact : alejandrodherme@gmail.com
*/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/
#ifndef DRIVERLPC_DR_IB_MOTORES_H_
#define DRIVERLPC_DR_IB_MOTORES_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/
#include "DR_Infotronic.h"
#include "DR_tipos.h"

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/
#define 	MARTILLO			RELAY3
#define	SENSOR_PAPEL			ED0
#define	SENSOR_FIN_CARRO		ED1

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/
typedef enum { FULL_STEP , FULL_STEP_POWER } motor_drive_t;

typedef enum { ENERGIZANDO , DESENERGIZANDO , DESUSO }   motor_estado_t;

typedef struct
{
	motor_drive_t tipo_drive; // typedef enum: FULL_STEP, FULL_STEP_POWER, HALF_STEP
	uint8_t puertos[4][2];	  // Fila (A-B-C-D) - Columna (0:Puerto-1:Bit)
	motor_estado_t estado_corriente; // typedef enum: ENERGIZANDO, DESENERGIZANDO, DESUSO
	int8_t tiempo_transcurrido;	 // en ticks
	uint8_t tiempo_on;		//tiempo que hay que prender la bobina
	uint8_t tiempo_off;		//tiempo que hay que apagar la bobina
	int8_t  estado_bob;		 // estado de la secuencia de bobinas
	int16_t pasos_pendientes;
	int16_t posicion;

} motorpap_t;

typedef struct
{
	motor_estado_t estado_corriente;
	uint8_t tiempo_transcurrido;	//en ticks
	uint8_t  tiempo_on;
	uint8_t  tiempo_off;

} motor_solenoide_t;

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/
extern __RW motorpap_t  MotorCabezal, MotorHoja;
extern __RW motorpap_t* MotoresPaP[2];
extern __RW bool_t 	   hw_motores_inicializado;
extern __RW motor_solenoide_t Martillo;

/***********************************************************************************************************************************
 *** PROTOTIPOS DE FUNCIONES GLOBALES
 **********************************************************************************************************************************/
void motores_inicializacion( void );
void actualizar_motores    ( void );
void reiniciar_motor( uint8_t motor);

#endif /* DRIVERLPC_DR_IB_MOTORES_H_ */
