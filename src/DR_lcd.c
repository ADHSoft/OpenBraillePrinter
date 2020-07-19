/*
 * Open Braille Printer (C) 2017-2019 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

#include "DR_lcd.h"
#include "DR_Infotronic.h"
#include "DR_gpio.h"
#include "DR_pinsel.h"

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
__RW uint8_t Buffer_LCD[ TAMANO_BUFFER_LCD ]; // Buffer del LCD
__RW int32_t inicio_buffer_lcd = 0; 	// Indice del buffer del LCD
__RW int32_t cantidad_cola_lcd = 0;  // Cantidad de datos en el buffer del LCD

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
uint8_t Demora_LCD = 10;			// este numero inicializarlo a menos en 5 por el delay de power on.
uint8_t flag;

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
 * @fn      void lcd_inicializacion( void )
 * @brief   Funcion encargada de inicializar el LCD
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void lcd_inicializacion( void )
{
//  Configuracion de los puertos para el LCD
	SetPINSEL( LCD_D4 , FUNCION_GPIO );
	SetDIR   ( LCD_D4 , SALIDA );
	SetPIN   ( LCD_D4 , OFF);

	SetPINSEL( LCD_D5 , FUNCION_GPIO);
	SetDIR   ( LCD_D5 , SALIDA );
	SetPIN   ( LCD_D5 , OFF);

	SetPINSEL( LCD_D6 , FUNCION_GPIO);
	SetDIR   ( LCD_D6 , SALIDA );
	SetPIN   ( LCD_D6 , OFF);

	SetPINSEL( LCD_D7 , FUNCION_GPIO);
	SetDIR   ( LCD_D7 , SALIDA );
	SetPIN   ( LCD_D7 , OFF);

	SetPINSEL( LCD_EN , FUNCION_GPIO);
	SetDIR   ( LCD_EN , SALIDA );
	SetPIN   ( LCD_EN , OFF);

	SetPINSEL( LCD_RS , FUNCION_GPIO);
	SetDIR   ( LCD_RS , SALIDA );
	SetPIN   ( LCD_RS , OFF);

//  Configuracion inicial
	PushLCD( 0x33 , LCD_CONTROL );
	PushLCD( 0x32 , LCD_CONTROL );

//  A partir de aca el LCD pasa a 4 bits
	PushLCD( 0b00101000 , LCD_CONTROL ); // 0010 1000 -> DL=0(4 bits)-N=1(2 lineas)-F=0(5x7 puntos)
	PushLCD( 0x08 , LCD_CONTROL ); // 0000 1000 -> D=0(Display OFF)-C=0(Cursor OFF)-B=0(Sin parpadeo)
	PushLCD( 1 , LCD_CONTROL ); // Clear Display
	PushLCD( 6 , LCD_CONTROL ); // 0000 0110 -> I/D=1(Desplazamiento a derecha)-S=0(Display quieto)
	PushLCD( 0x0F , LCD_CONTROL ); // 0000 1111 -> D=1(Display ON)-C=1(Cursor ON)-B=1(Con parpadeo)
}

 /******************************************************************************************************************
  * @fn		 void DriverDemoraLCD( void )
  * @brief   Funcion encargada de la demora inicial y es la que llama a la funcion actualizar_lcd()
  * @param:  void
  * @return: void
  *****************************************************************************************************************/
void DriverDemoraLCD( void )
{

//  La primera vez cuenta 25ms
	if( Demora_LCD != 0 )
	{   // Esto forma parte del while que pierde tiempo para la inicializacion del LCD.
		if( Demora_LCD == 1 )
			SetPIN( LCD_EN , OFF );	// Enable=0 (Se deshabilita 2.5ms)
		Demora_LCD--;
	}
	else
	{
		Demora_LCD = 1;
		actualizar_lcd(); // Actualiza cada 5ms
	}

}

/******************************************************************************************************************
 * @fn      void actualizar_lcd( void )
 * @brief   Funcion que refresca el display cada 5ms
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void actualizar_lcd( void )
{
	int32_t data;

	if( ( data = PopLCD() ) == NO_DATA )
		return;

	SetPIN( LCD_D7 , ( ( (uint8_t) data ) >> 3 ) & 1 );
	SetPIN( LCD_D6 , ( ( (uint8_t) data ) >> 2 ) & 1 );
	SetPIN( LCD_D5 , ( ( (uint8_t) data ) >> 1 ) & 1 );
	SetPIN( LCD_D4 , ( ( (uint8_t) data ) >> 0 ) & 1 );

	if( ( (uint8_t)data ) & 0x80 )
		SetPIN( LCD_RS , OFF ); // RS=0 (Control)
	else
		SetPIN( LCD_RS , ON  ); // RS=1 (Dato)

	SetPIN( LCD_EN , ON );  // Enable=1 (Se habilita 2.5ms)
}

/******************************************************************************************************************
 * @fn      int32_t PopLCD( void )
 * @brief   Funcion encargada de sacar un dato del buffer del LCD
 * @param:  void
 * @return: Devuelve el dato o NO_DATA (-1) si no hay dato en el buffer
 *****************************************************************************************************************/
 int32_t PopLCD( void )
{
	int32_t dato;

	if( cantidad_cola_lcd == 0 )  // Si el buffer esta vacio
		return NO_DATA;

	dato = Buffer_LCD[ inicio_buffer_lcd ];

	cantidad_cola_lcd--;

	inicio_buffer_lcd++;
	inicio_buffer_lcd %= TAMANO_BUFFER_LCD;

	return dato;
}

/******************************************************************************************************************
 * @fn      void BorrarLCD( void )
 * @brief   Borra el display y posiciona el cursor al principio
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void BorrarLCD( void )
{
	//para ahorar tiempo y no escribir cosas que seran borradas, se borran los comandos anteriores
	//, tomando precaucion de enviar el ultimo nibble si faltaba enviar uno
	//Si hay menos de 12 bytes, por las dudas de no borrar el comando de inicializacion, no borrar el buffer.

	if (cantidad_cola_lcd>24)
		cantidad_cola_lcd=cantidad_cola_lcd%2;

	PushLCD( 1 , LCD_CONTROL ); // Clear Display
}


