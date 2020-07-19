/*******************************************************************************************************************************//**
 *
 * @file		Infotronic.h
 * @brief		Macros, tipos , prototipos, de la aplicacion
 * @date		23-03-16
 * @author		Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/
#ifndef DR_GPIO_H_
#define DR_GPIO_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/
#include "DR_tipos.h"

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/
//!< Entradas - Salidas
#define	 GPIO  ( ( __RW uint32_t * )  0x2009C000 )

#define	 FIO0DIR	GPIO[ 0 ]
#define	 FIO0MASK	GPIO[ 4 ]
#define	 FIO0PIN	GPIO[ 5 ]
#define	 FIO0SET	GPIO[ 6 ]
#define	 FIO0CLR	GPIO[ 7 ]
#define	 FIO1DIR	GPIO[ 8 ]
#define	 FIO1MASK	GPIO[ 12 ]
#define	 FIO1PIN	GPIO[ 13 ]
#define	 FIO1SET	GPIO[ 14 ]
#define	 FIO1CLR	GPIO[ 15 ]
#define	 FIO2DIR	GPIO[ 16 ]
#define	 FIO2MASK	GPIO[ 20 ]
#define	 FIO2PIN	GPIO[ 21 ]
#define	 FIO2SET	GPIO[ 22 ]
#define	 FIO2CLR	GPIO[ 23 ]
#define	 FIO3DIR	GPIO[ 24 ]
#define	 FIO3MASK	GPIO[ 28 ]
#define	 FIO3PIN	GPIO[ 29 ]
#define	 FIO3SET	GPIO[ 30 ]
#define	 FIO3CLR	GPIO[ 31 ]
#define	 FIO4DIR	GPIO[ 32 ]
#define	 FIO4MASK	GPIO[ 36 ]
#define	 FIO4PIN	GPIO[ 37 ]
#define	 FIO4SET	GPIO[ 38 ]
#define	 FIO4CLR	GPIO[ 39 ]

//!< Registros de modo de funcionamiento las gpio
#define	 PINMODE  		( ( __RW uint32_t * )  0x4002C040 )

#define	 PINMODE0	PINMODE[0]
#define	 PINMODE1	PINMODE[1]
#define	 PINMODE2	PINMODE[2]
#define	 PINMODE3	PINMODE[3]
#define	 PINMODE4	PINMODE[4]
#define	 PINMODE5	PINMODE[5]		//!< No esta en uso
#define	 PINMODE6	PINMODE[6]		//!< No esta en uso
#define	 PINMODE7	PINMODE[7]
#define	 PINMODE8	PINMODE[8]		//!< No esta en uso
#define	 PINMODE9	PINMODE[9]

#define	 PINMODE_OD		( ( __RW uint32_t * )  0x4002C068 )

#define	 PINMODE_OD0	PINMODE_OD[0]
#define	 PINMODE_OD1	PINMODE_OD[1]
#define	 PINMODE_OD2	PINMODE_OD[2]
#define	 PINMODE_OD3	PINMODE_OD[3]
#define	 PINMODE_OD4	PINMODE_OD[4]


///////////////////////////////////////////////////   PCONP   ////////////////////////////////////////////////////////////////

// Power Control for Peripherals register (PCONP - 0x400F C0C4)
// 0x400FC0C4UL: Direccion de inicio del registro de habilitación de dispositivos:
	#define 	PCONP	(* ( ( __RW uint32_t  * ) 0x400FC0C4UL ))

//////////////////////////////////////////////////   PCLKSEL   //////////////////////////////////////////////////////////////

// Peripheral Clock Selection registers 0 and 1 (PCLKSEL0 -0x400F C1A8 and PCLKSEL1 - 0x400F C1AC)
// 0x400FC1A8UL: Direccion de inicio de los registros de seleccion de los CLKs de los dispositivos:
	#define		PCLKSEL		( ( __RW uint32_t  * ) 0x400FC1A8UL )

//  Registros PCLKSEL
	#define		PCLKSEL0	PCLKSEL[0]
	#define		PCLKSEL1	PCLKSEL[1]

////////////////////////////////////////////////   ISER - ICER   ///////////////////////////////////////////////////////////

// Registros de habilitación (set) de interrupciones en el NVIC:
	#define		ISER		( ( uint32_t * ) 0xE000E100UL )

// Registros de deshabilitacion (clear) de interrupciones en el NVIC:
	#define		ICER		( ( uint32_t * ) 0xE000E180UL )


// Registros ISER: Para habilitar las Interrupciones. Se activan con 1. Escribiendo un 0 no hace nada

    #define		ISER0		ISER[0]
	#define		ISER1		ISER[1]
	#define		ISE_EINT3	ISER[0] |= ( 0x00000001 << 21 )  //ISER0->bit21 (1 en el bit 21 habilita la EINT3)
    #define     ISE_EINT2	ISER[0] |= ( 0x00000001 << 20 )  //ISER0->bit20 (1 en el bit 21 habilita la EINT2)
    #define     ISE_EINT1	ISER[0] |= ( 0x00000001 << 19 )  //ISER0->bit19 (1 en el bit 21 habilita la EINT1)
    #define     ISE_EINT0	ISER[0] |= ( 0x00000001 << 18 )  //ISER0->bit18 (1 en el bit 21 habilita la EINT0)


// Registros ICER: Para deshabilitar las Interupciones. Se desactivan con 1. Escribiendo un 0 no hace nada

	#define		ICER0		ICER[0]
	#define		ICER1		ICER[1]
	#define		ICE_EINT3	ICER0 |= ( 0x00000001 << 21 ) 	// deshabilito a EINT3
    #define		ICE_EINT2	ICER0 |= ( 0x00000001 << 20 ) 	// deshabilito a EINT2
    #define		ICE_EINT1	ICER0 |= ( 0x00000001 << 19 ) 	// deshabilito a EINT1
    #define		ICE_EINT0	ICER0 |= ( 0x00000001 << 18 )	// deshabilito a EINT0


///////////////////////////////////// REGISTROS PARA INTERRUPCIONES EXTERNAS //////////////////////////////////////////////

// Registro EXTINT: Registro de FLAGS para limpiar la ISR. Se escribe un 1 para borrar

	#define		EXTINT 		( (uint32_t  * ) 0x400FC140UL )

	#define		CLR_EINT3		EXTINT[0] |= ( 0x00000001 << 3 ) // bajo el flag de EINT3
    #define		CLR_EINT2		EXTINT[0] |= ( 0x00000001 << 2 ) // bajo el flag de EINT2
    #define		CLR_EINT1		EXTINT[0] |= ( 0x00000001 << 1 ) // bajo el flag de EINT1
    #define		CLR_EINT0		EXTINT[0] |=   0x00000001        // bajo el flag de EINT0

// Registro EXTMODE: Para seleccionar si la ISR Externa activa por flanco ó nivel. Por defecto es por nivel
	#define		EXTMODE 		( (uint32_t  * ) 0x400FC148 )

	#define		EXTMODE3_F		EXTMODE[0] |= ( 0x00000001 << 3 )  // EINT3 por flanco
    #define		EXTMODE2_F		EXTMODE[0] |= ( 0x00000001 << 2 )  // EINT2 por flanco
    #define		EXTMODE1_F		EXTMODE[0] |= ( 0x00000001 << 1 )  // EINT1 por flanco
    #define		EXTMODE0_F		EXTMODE[0] |=   0x00000001         // EINT0 por flanco

// Registro EXTPOLAR: Selecciona polaridad del EXTMODE. Por defecto es por Flanco ó Nivel Negativo
	#define    EXTPOLAR        ( (uint32_t  * ) 0x400FC14C )

    #define    EXTPOLAR3_P      EXTPOLAR[0] |= ( 0X00000001 << 3 ) // Flanco ó Nivel Positivo
    #define    EXTPOLAR2_P      EXTPOLAR[0] |= ( 0X00000001 << 2 ) // Flanco ó Nivel Positivo
    #define    EXTPOLAR1_P      EXTPOLAR[0] |= ( 0X00000001 << 1 ) // Flanco ó Nivel Positivo
    #define    EXTPOLAR0_P      EXTPOLAR[0] |=  0X00000001         // Flanco ó Nivel Positivo


/////////////////////////////////////// REGISTROS DE GPIO PARA USARSE COMO ISR ///////////////////////////////////////////

//  Configuración
    #define   IO0IntEnR   ( (uint32_t *) 0x40028090UL ) // Puerto 0 como flanco ascendente
    #define   IO2IntEnR   ( (uint32_t *) 0x400280B0UL ) // Puerto 2 como flanco ascendente
    #define   IO0IntEnF   ( (uint32_t *) 0x40028094UL ) // Puerto 0 como flanco descendente
    #define   IO2IntEnF   ( (uint32_t *) 0x400280B4UL ) // Puerto 2 como flanco descendente

//  Estado
    #define   IO0IntStatR ( (uint32_t *) 0x40028084UL ) //Estado de los flags de interr flanco ascendente bits Puerto 0
    #define   IO2IntStatR ( (uint32_t *) 0x400280A4UL ) //Estado de los flags de interr flanco ascendente bits Puerto 2
    #define   IO0IntStatF ( (uint32_t *) 0x40028088UL ) //Estado de los flags de interr flanco descendente bits Puerto 0
    #define   IO2IntStatF ( (uint32_t *) 0x400280A8UL ) //Estado de los flags de interr flanco descendente bits Puerto 2
    #define   IOIntStatus ( (uint32_t *) 0x40028080UL ) //Estado de los flags de interr de bits Puerto 2 y Puerto 0

//  Bajo flags de Interr por GPIO
    #define   IO0IntClr   ( (uint32_t *) 0x4002808CUL ) // Bajo flags de Interr Puerto 0
	#define   IO2IntClr   ( (uint32_t *) 0x400280ACUL ) // Bajo flags de Interr Puerto 2

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define 	ALTO	1
#define 	BAJO	0
#define 	SALIDA	1
#define 	ENTRADA	0

#define 	FUNCION_GPIO		0

#define 	PULLUP				0
#define 	REPEATER			1
#define 	NONE				2
#define 	PULLDOWN			3

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/


/***********************************************************************************************************************************
 *** PROTOTIPOS DE FUNCIONES GLOBALES
 **********************************************************************************************************************************/
void  SetDIR  ( uint8_t puerto , uint8_t bit , uint8_t dir );
void  SetMODE ( uint8_t puerto , uint8_t bit , uint8_t modo );
void  SetPIN  ( uint8_t puerto , uint8_t bit , uint8_t estado );
uint8_t GetPIN( uint8_t puerto , uint8_t bit , uint8_t actividad );

#endif /* DR_GPIO_H_ */
