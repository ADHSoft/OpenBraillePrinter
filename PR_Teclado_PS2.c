/*
 * Open Braille Printer (C) 2017-2019 Alejandro Herme.
 * See LICENSE.txt for more details. Contact : alejandrodherme@gmail.com
*/

#include "PR_Teclado_PS2.h"
#include "DR_TecladoPS2.h"
#include "AP_Funciones.h"


#define CSB_1 0x2b //F D S J K L
#define CSB_2 0x23
#define CSB_3 0x1B
#define CSB_4 0x3b
#define CSB_5 0x42
#define CSB_6 0x4b //L
#define CSB_ACCEPT 0x29	//SPACE

#define CANTIDAD_CODIGOS 51
// Ñ = 0xEE
uint8_t const codigo_rastreo[CANTIDAD_CODIGOS][2] = {
	0x45,'0', 0x16,'1', 0x1e,'2', 0x26,'3', 0x25,'4', 0x2e,'5', 0x36,'6', 0x3d,'7', 0x3e,'8', 0x46,'9',
	0x1C,'a', 0x32,'b', 0x21,'c', 0x23,'d', 0x24,'e', 0x2b,'f', 0x34,'g', 0x33,'h', 0x43,'i', 0x3b,'j',
	0x42,'k', 0x4b,'l', 0x3a,'m', 0x31,'n', 0x4c,0xEE, 0x44,'o', 0x4d,'p', 0x15,'q', 0x2d,'r', 0x1B,'s',
	0x2c,'t', 0x3c,'u', 0x2a,'v', 0x1D,'w', 0x22,'x', 0x35,'y', 0x1A,'z', BLOQ_MAYUS, '^', TAB, '#', ENTER, '\n',
	0x41,',', 0x49,'.', 0x4a,'_', 0x4e,'?', 0x5D,';', 0x55,'!', 0x54,'(', 0x52,')', 0x0e,'"', 0x5b,'*', 0x29 ,' '
};

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 **********************************************************************************************************************************/
	__RW uint8_t Tecla_Letra=0, Tecla_Funcion=0, CSBChar=0, kbd_mode=KBD_FULL_KEYBOARD;


uint8_t ch_kbd_mode(uint8_t mode){
	CSBChar=0;
	kbd_mode=mode;
	return kbd_mode;
}


/*********************************************************************************************
 * @fn     void decode_PS2( uint8_t reset )
 * @brief  Decodificación de pulsaciones
 * @detail Decodifica la pulsación realizada, convirtiendola a un caracter de la tabla
 * @param  reset
 * @return void
 ********************************************************************************************/
void interpretar_buffer_PS2( uint8_t reset )
{
	static uint8_t code_break = 0;	// Flag para indicar que la tecla se está soltando
	static uint8_t code_Extendido = 0;
	uint8_t i, codigo;

	if (reset) {
		code_break=0;
		code_Extendido=0;
		cantidadRX=0;
		cantidadTX=0;
		return;
	}

	if ( Tecla_Funcion !=0 || Tecla_Letra!=0 )
		return;  //la aplicacion no está lista, debe resetear estas variables para indicar que lo está.

	if (cantidadRX==0) {
		return;			//nada que hacer
	} else {
		codigo=BufferPS2RX[IndexStartRX];	//leer del buffer
		IndexStartRX=(IndexStartRX+1)%TAMANO_RX;
		cantidadRX--;
	}

	if( codigo == 0xE0 )
	{
		code_Extendido = 1;
		return;
	}

	if ( codigo == 0xF0) {
		code_break=1;
		return;
	}

	//Si el codigo no fue 0xF0 ni 0xE0 realizar:

	if( code_break==1 )	//solo responder cuando se levanta una tecla, no cuando se aprieta, para evitar repeticiones.
	{
		if( code_Extendido == 1) {

			Tecla_Funcion=codigo;	//si es un codigo extendido, seguro no es una tecla de caracter

		} else {

			if ( kbd_mode==KBD_CLASSIC_SIXBUTTONS ) {
				// borrar puntos a CSBChar (tecla levantada)
				switch (codigo) {
				case CSB_1: CSBChar&=~(1<<0);
							break;
				case CSB_2: CSBChar&=~(1<<1);
							break;
				case CSB_3: CSBChar&=~(1<<2);
							break;
				case CSB_4: CSBChar&=~(1<<3);
							break;
				case CSB_5: CSBChar&=~(1<<4);
							break;
				case CSB_6: CSBChar&=~(1<<5);
							break;
				case CSB_ACCEPT:
					CSBChar&=0b00111111;	//siempre borrar los 2 bits no usados
					Tecla_Letra=braille_to_ascii(CSBChar);
					break;
				default:
					Tecla_Funcion=codigo;
					break;
				}
			} else {
				//KBD_Mode = normal
				// Buscar el caracter de la tecla:
				for( i = 0 ; i<CANTIDAD_CODIGOS  ; i++ ) {
					if( codigo_rastreo[i][0] == codigo )
					{
						Tecla_Letra=codigo_rastreo[i][1];
						break;
					}
				}

				if (Tecla_Letra==0) {
					Tecla_Funcion=codigo;	//si no es ningun caracter, guardar como tecla de funcion
				}
			}

		}
	} else {	//solo responder a una tecla levantada. A menos que se este en modo de 6 botones:
		if ( kbd_mode==KBD_CLASSIC_SIXBUTTONS ) {
			// agregar puntos a CSBChar
			switch (codigo) {
			case CSB_1: CSBChar|=(1<<0);
				break;
			case CSB_2: CSBChar|=(1<<1);
				break;
			case CSB_3: CSBChar|=(1<<2);
				break;
			case CSB_4: CSBChar|=(1<<3);
				break;
			case CSB_5: CSBChar|=(1<<4);
				break;
			case CSB_6: CSBChar|=(1<<5);
				break;
			}
		}

	}

	code_Extendido = 0;
	code_break = 0;

}
