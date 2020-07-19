/*
 * Open Braille Printer (C) 2017-2019 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "PR_lcd.h"
#include "DR_lcd.h"
#include "AP_Buffer.h"
#include "PR_Timers.h"

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

/******************************************************************************************************************
 * @fn      void lcd_tipo_cursor( int8_t cursor, int8_t despl, int8_t parpadeo )
 * @brief   Selecciona el tipo de cursor
 * @param:  cursor: ON=1 - OFF=0
 * @param:  parpadeo: ON=1 - OFF=0
 * @return: void
 *****************************************************************************************************************/
void lcd_tipo_cursor( int8_t cursor, int8_t parpadeo )
{
	if( cursor == OFF && parpadeo == OFF  )
		PushLCD( 0x0C , LCD_CONTROL ); // 0000 1100->D=1(Display ON)-C=0(Cursor OFF)-B=0(Sin parpadeo)

	if( cursor == ON  && parpadeo == OFF  )
		PushLCD( 0x0E , LCD_CONTROL ); // 0000 1110->D=1(Display ON)-C=1(Cursor ON )-B=0(Sin parpadeo)

	if( cursor == OFF && parpadeo == ON   )
		PushLCD( 0x0D , LCD_CONTROL ); // 0000 1101->D=1(Display ON)-C=0(Cursor OFF)-B=1(Con parpadeo)

	if( cursor == ON  && parpadeo == ON   )
		PushLCD( 0x0F , LCD_CONTROL ); // 0000 1111->D=1(Display ON)-C=1(Cursor ON )-B=1(Con parpadeo)
}

/******************************************************************************************************************
 * @fn      uint8_t PushLCD( uint8_t dato , uint8_t control , uint8_t medio_byte )
 * @brief   Funcion para poner un dato en el buffer del LCD
 * @param   dato:    Dato a poner en el buffer del LCD
 * @param   control: Indica si es un dato de status o de control
 * @param   medio_byte: Indica si se envia un HALF_BYTE O FULL_BYTE
 * @return  Devuelve -1 en caso de error, o 0 en caso contrario
 *****************************************************************************************************************/
int16_t PushLCD( uint8_t dato , uint8_t control ) // manda el dato
{
	int32_t posicion; // Lugar del buffer del LCD donde se pondra el dato

	if( cantidad_cola_lcd >= ( TAMANO_BUFFER_LCD - 2 ) )
		return -1;

	posicion  = inicio_buffer_lcd + cantidad_cola_lcd;
	posicion %= TAMANO_BUFFER_LCD;
	Buffer_LCD[ posicion ] = ( dato >> 4 ) & 0x0F;	//primero el nibble mas significativo

	if( control == LCD_CONTROL )
		Buffer_LCD[ posicion] |= 0x80; // El 0x80 es para acceder a la memoria DD RAM
	cantidad_cola_lcd++;

	posicion  = inicio_buffer_lcd + cantidad_cola_lcd;
	posicion %= TAMANO_BUFFER_LCD;
	Buffer_LCD[ posicion ] = dato & 0x0F;

	if( control == LCD_CONTROL )
		Buffer_LCD[ posicion ] |= 0x80; // Agrega un 1 en el bit LCD_D7 para acceder a DD RAM
	cantidad_cola_lcd ++;

	return 0;
}

void lcd_mostrar_buffer_editable(){
	lcdprintf(MOSTRAR_BUFFER_EDITABLE);
}

/*********************************************************************************************
 * @fn     void lcdprintf( int8_t *msg , int8_t linea , int8_t pos )
 * @brief  Funcion para escribir un mensaje en el LCD
 * @param  msg: Strings del mensaje a escribir (primera y segunda linea). Poner NULL para mostrar buffer(zona editable).
 ********************************************************************************************/
void lcdprintf( char *linea1 , char *linea2 )
{
	#define SIMBOLO_ENTER 0x7F

	uint8_t i, lcd_borrado=0;

	if ( linea1==NULL && linea2==NULL ) {	//se desea mostrar el buffer
		BorrarLCD();
		lcd_tipo_cursor(ON,ON);
		TimerStop(14);
		linea1=buffer_get_string_editable();
		PushLCD( 0x80 , LCD_CONTROL ); //1er Renglon
		for (i=0;linea1[i] != '\0';i++) {
			if (linea1 [i]!='\n') {
				PushLCD (linea1 [i] , LCD_DATA);
			} else {
				PushLCD ( SIMBOLO_ENTER , LCD_DATA );
			}
			if (i==15) PushLCD( 0xc0 , LCD_CONTROL ); // 2do Renglon

		}

	} else {
		if (linea1!=NULL && linea2!=NULL) {
			BorrarLCD();
			lcd_borrado=1;
		}
		//se desea mostrar un mensaje (se lo mantiene 3 segundos)
		lcd_tipo_cursor(OFF,OFF);
		TimerStart(14,3,SEG,lcd_mostrar_buffer_editable);	//luego de mostrar msj por 3 seg, volver a mostrar buffer.

		if ( linea1!=NULL ) {

			PushLCD( 0x80 , LCD_CONTROL ); // 1er Renglon

			for (i=0;linea1[i] != '\0';i++) {
				PushLCD (linea1 [i] , LCD_DATA);
			}
			if (lcd_borrado==0){
				for (;i<16;i++) {	//borrar resto de la linea
					PushLCD (' ' , LCD_DATA);
				}
			}

		}

		if ( linea2!=NULL ) {

			PushLCD( 0xc0 , LCD_CONTROL ); // 2do Renglon

			for (i=0;linea2[i] != '\0';i++) {
				PushLCD (linea2 [i] , LCD_DATA);
			}
			if (lcd_borrado==0){
				for (;i<16;i++) {	//borrar resto de la linea
					PushLCD (' ' , LCD_DATA);
				}
			}
		}
	}
}


/*********************************************************************************************
 * @fn     void GuardarMensajeLCD( char *mensaje, char *lugar )
 * @brief  Funcion que rellena el mensaje a enviar con espacios vacios asi se 'borran' los anteriores lugares del LCD
 * @param  mensaje:  Puntero al mensaje a escribir
 * @param  lugar:    Puntero al lugar donde se quiere guardar el mensaje pasado
 * @return void
 ********************************************************************************************/
void GuardarMensajeLCD( char *mensaje, char *lugar )
{
	uint8_t i;

	for( i = 0; mensaje[i] != 0; i++ )
		lugar[i] = mensaje[i];

	for( ; i < 16; i++ )
		lugar[i] = ' ';

	lugar[17] = 0;

}















