/*
 * Open Braille Printer (C) 2017-2019 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/
#ifndef PRIMITIVAS_PR_MOTORES_H_
#define PRIMITIVAS_PR_MOTORES_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/
#define 	MOTOR_CARRO 	0
#define 	MOTOR_HOJA 		1


#define POS_X_PRIMERA_LETRA 	380
#define ANCHO_LETRA 			10
#define ANCHO_ESPACIO 			15	//10
#define ALTO_MARGEN_SUP 		610
#define ALTO_ESPACIO_LINEA 		45	//100
#define ALTO_LETRA 				(ALTO_ESPACIO_LINEA*2)
#define ALTO_SEPARACION_RENGLON 100
#define LETRAS_POR_RENGLON 		((POS_X_PRIMERA_LETRA/(ANCHO_LETRA+ANCHO_ESPACIO))-1)		//14
#define RENGLONES_POR_HOJA 		10
#define LETRAS_POR_HOJA			RENGLONES_POR_HOJA*LETRAS_POR_RENGLON	//140


/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** PROTOTIPOS DE FUNCIONES GLOBALES
 **********************************************************************************************************************************/
void   mover_motor( uint8_t motor , int16_t pasos );
bool_t motor_listo( uint8_t motor );
void   parada_emergencia_motores( void );
void   martillar( void );
bool_t motores_parados();
bool_t posicionar_motor_y_martillar( int16_t pos_x , bool_t  );
bool_t calibracion_cabezal();

#endif /* PRIMITIVAS_PR_MOTORES_H_ */
