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
#include "DR_UART.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/

#define 	TX1 	0 , 15
#define 	RX1 	0 , 16
#define 	TX0 	0 , 2
#define 	RX0 	0 , 3

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

__RW uint8_t IndiceTxStart  = 0;
__RW uint8_t CantidadTx = 0;

__RW uint8_t IndiceRxStart  = 0;
__RW uint8_t CantidadRx = 0;

__RW uint8_t BufferTx[ TOPE_BUFFER_TX ];
__RW uint8_t BufferRx[ TOPE_BUFFER_RX ];

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
static __RW uint8_t enviando_flag=0;
/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/

/*********************************************************************************************
 * @fn     void UART_PushRx( uint8_t dato )
 * @brief  Pone datos que recibe del exterior, en el buffer de recepción
 * @param  dato: valor que se recibe
 * @return void
 ********************************************************************************************/
void UART0_PushRx( uint8_t dato )
{
	if (CantidadRx==TOPE_BUFFER_RX) {
		//buffer lleno
	} else {
		BufferRx[ (IndiceRxStart + CantidadRx) % TOPE_BUFFER_RX ] = dato;

		CantidadRx ++;
	}
}

/*********************************************************************************************
 * @fn     void UART0_Iniciar_Envio( uint8_t dato )
 * @brief  Avisa al modulo UART que se quiere enviar datos.
 * @param  void
 * @return void
 ********************************************************************************************/
void UART0_Iniciar_Envio(  ){
	if (!enviando_flag){	//si no esta activo el envio de datos
		REGISTRO_DE_TRANSMISION = (uint32_t) UART0_PopTx();//enviar el primer dato manualmente REGISTRO_DE_TRANSMISION=dato;
		enviando_flag=1;
	}
}

/*********************************************************************************************
 * @fn     int16_t UART_PopTx( void )
 * @brief  Saca los datos a transmitir, del buffer de transmisión
 * @param  void
 * @return Devuelve el dato a transmitir
 ********************************************************************************************/
int16_t UART0_PopTx( void )
{
	int16_t dato;

	if( CantidadTx == 0 )
		return -1;

	dato = (uint16_t) BufferTx[ IndiceTxStart ];
	IndiceTxStart ++;
	IndiceTxStart %= TOPE_BUFFER_TX;
	CantidadTx--;

	return dato;
}

/*********************************************************************************************
 * @fn     void InitUART0( void )
 * @brief  Inicializa la UART0 (Para conexion PC)
 * @param  void
 * @return void
 ********************************************************************************************/
void InitUART0( void )	//uart PC
{
	PCONP    |=  ( 0x01 << 3 );	// Bit 3 en 1 prende la UART0
	PCLKSEL0 &= ~( 0x03 << 8 ); // Bit 8 y 9 en 0 selecciona que el (p)clk de la UART0 sea CCLK/4 = 25MHz

	U0LCR = 0b10000000; // Habilitamos DLAB = 1

	//baudrate = PCLK / (16*(256*DLM+DLL))
	U0DLM = 0;
	U0DLL = 0xA3;  // 9600 Baudios , Esto debe coincidir con las opciones del driver en la PC

	SetPINSEL( TX0 , PINSEL_FUNC1 );	// funcion 1 = UART
	SetPINSEL( RX0 , PINSEL_FUNC1 );	// funcion 1 = UART

	U0LCR = 0b00001011; // ( 8b, Impar, 1b stop )

	U0IER  = 3;       // Habilito interrupcion para cuando se reciba un dato o se pueda enviar uno
	ISER0 |= ( 1 << 5 ); // Habilito interrupciones del UART0 en el NVIC
}

/*********************************************************************************************
 * @fn     void UART0_IRQHandler( void )
 * @brief  Interrupcion de la UART
 * @param  void
 * @return void
 ********************************************************************************************/
void UART0_IRQHandler( void )
{
	uint8_t iir;
	int32_t dato;

	do
	{
		iir = U0IIR; // IIR es reset por HW (Lo leo y se borra)

		switch( ( iir >> 1 ) & 3 )
		{
			case IIR_RDA: // Se recibio un dato
				dato = REGISTRO_DE_RECEPCION;
				UART0_PushRx( (int32_t) dato );
				break;

			case IIR_THRE: // THR disponible
				dato = UART0_PopTx();
				if( dato != -1 ) {
					REGISTRO_DE_TRANSMISION = (uint32_t) dato;
				} else {
					enviando_flag=0;
				}

				break;
		}
	}
	while( !( iir & 1 ) );	//hasta que no quede ninguna interrupcion pendiente
}
