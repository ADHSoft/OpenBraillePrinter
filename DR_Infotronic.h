/*
 * Open Braille Printer (C) 2017-2019 Alejandro Herme.
 * See LICENSE.txt for more details. Contact : alejandrodherme@gmail.com
*/

#ifndef DRIVERINFOTRONIC_DR_INFOTRONIC_H_
#define DRIVERINFOTRONIC_DR_INFOTRONIC_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/

#define		RELAY1				2,0
#define		RELAY2				0,23
#define		RELAY3				0,21
#define		RELAY4				0,27
#define 	RGB_B				2,1
#define 	RGB_G				2,2
#define 	RGB_R				2,3
#define 	BUZZER				0,28
#define 	SW5					1,26	//se comparte con señal ED0 (entrada digital)
#define 	SW4					2,13
#define 	SW3					0,11
#define 	SW2					0,18
#define 	SW1					2,10	//coincide con el boton ISP del LPC
#define 	LCD_D4					0,5
#define 	LCD_D5					0,10
#define 	LCD_D6					2,4
#define 	LCD_D7					2,5
#define 	LCD_EN					0,4
#define 	LCD_RS					2,6

#define EXPANSION0	2,7
#define EXPANSION1	1,29
#define EXPANSION2	4,28
#define EXPANSION3	1,23
#define EXPANSION4	1,20
#define EXPANSION5	0,19
#define EXPANSION6	3,26	//coincide con LPC_RGB_B
#define EXPANSION7	1,25
#define EXPANSION8	1,22
#define EXPANSION9	1,19
#define EXPANSION10	0,20
#define EXPANSION11	3,25	//coincide con LPC_RGB_G
#define EXPANSION12	1,27
#define EXPANSION13	1,24
#define EXPANSION14	1,21
#define EXPANSION15	1,18

#define 	TX1 	0 , 15
#define 	RX1 	0 , 16
#define 	RTS1 	0 , 22	//coincide con LPC_RGB_R
#define 	CTS1 	0 , 17
#define 	TX0 	0 , 2
#define 	RX0 	0 , 3

#define EXPANSION16	2,8
#define EXPANSION17	2,12
#define EXPANSION18	RX1
#define EXPANSION19	TX1
#define EXPANSION20	RTS1	//coincide con LPC_RGB_R
#define EXPANSION21	CTS1

#define LPC_RGB_R	0,22	//coincide con RTS1(serie)
#define LPC_RGB_G	3,25	//coincide con EXPANSION11
#define LPC_RGB_B	3,26	//coincide con EXPANSION6

#define ED0			1,26	//Entradas Digitales
#define ED1			4,29
#define ED2			2,11

#define SALIDA_AUDIO			0,26

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/
// extern tipo nombreVariable;

/***********************************************************************************************************************************
 *** PROTOTIPOS DE FUNCIONES GLOBALES
 **********************************************************************************************************************************/

#endif /* DRIVERINFOTRONIC_DR_INFOTRONIC_H_ */
