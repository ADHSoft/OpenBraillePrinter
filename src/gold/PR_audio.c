/*
 * Open Braille Printer (C) 2017-2021 Adhsoft0@gmail.com
 * See LICENSE.txt for more details.
*/

/*
 * PR_audio.c
 *
 *  Created on: 24 de set. de 2017
 */

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "audio_data.h"
#include "DR_dac.h"
#include "PR_audio.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
#define 	CANTIDAD_AUDIOS 	14

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
static audio_t audios[ CANTIDAD_AUDIOS ];
static uint8_t audio_struct_inicializado = FALSE;


/***********************************************************************************************************************************
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/
void audio_struct_formatter();

 /***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/******************************************************************************************************************
 * @fn      void audio_struct_formatter( void )
 * @brief	Hace que las estructuras de audio apunten a lo que tienen que apuntar
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void audio_struct_formatter( )
{

	#ifndef AGREGAR_TODO_EL_AUDIO_AL_BINARIO

	uint8_t i;
	for( i = 0 ; i < CANTIDAD_AUDIOS ; i++ )
	{
		audios[i].Size     = pcms_pc;
		audios[i].pcm_data = pcmd_pc;
	}

	#else

	audios[AUDIO_PC].Size= pcms_pc;
	audios[AUDIO_PC].pcm_data=pcmd_pc;
	audios[AUDIO_INMEDIATO].Size= pcms_inmediato;
	audios[AUDIO_INMEDIATO].pcm_data=pcmd_inmediato;
	audios[AUDIO_LISTO_PARA_IMPRIMIR].Size= pcms_listo_para_imprimir;
	audios[AUDIO_LISTO_PARA_IMPRIMIR].pcm_data=pcmd_listo_para_imprimir;
	audios[AUDIO_BUFFER].Size= pcms_buffer;
	audios[AUDIO_BUFFER].pcm_data=pcmd_buffer;
	audios[AUDIO_ERROR_CRITICO].Size= pcms_error_critico;
	audios[AUDIO_ERROR_CRITICO].pcm_data=pcmd_error_critico;
	audios[AUDIO_MODO].Size= pcms_modo;
	audios[AUDIO_MODO].pcm_data=pcmd_modo;
	audios[AUDIO_NO_HAY_HOJA].Size= pcms_no_hay_hoja;
	audios[AUDIO_NO_HAY_HOJA].pcm_data=pcmd_no_hay_hoja;
	audios[AUDIO_HOJA_CARGADA].Size= pcms_hoja_cargada;
	audios[AUDIO_HOJA_CARGADA].pcm_data=pcmd_hoja_cargada;
	audios[AUDIO_NO_HAY_MAS_ESPACIO_EN_LA_HOJA].pcm_data=pcmd_no_hay_mas_espacio_en_hoja;
	audios[AUDIO_NO_HAY_MAS_ESPACIO_EN_LA_HOJA].Size=pcms_no_hay_mas_espacio_en_hoja;
	audios[AUDIO_ULTIMO_RENGLON].Size= pcms_ultimo_renglon;
	audios[AUDIO_ULTIMO_RENGLON].pcm_data=pcmd_ultimo_renglon;
	audios[AUDIO_SONIDO_ACTIVADO].pcm_data=pcmd_sonido_activado;
	audios[AUDIO_SONIDO_ACTIVADO].Size=pcms_sonido_activado;
	audios[AUDIO_SONIDO_TECLA].pcm_data=pcmd_sonido_de_tecla;
	audios[AUDIO_SONIDO_TECLA].Size=pcms_sonido_de_tecla;
	audios[AUDIO_NO_HAY_MAS_MEMORIA_PARA_ESCRIBIR].pcm_data=pcmd_no_hay_mas_memoria;
	audios[AUDIO_NO_HAY_MAS_MEMORIA_PARA_ESCRIBIR].Size=pcms_no_hay_mas_memoria;
	audios[AUDIO_BIENVENIDO].pcm_data=pcmd_bienvenido;
	audios[AUDIO_BIENVENIDO].Size=pcms_bienvenido;

	#endif

}

 /***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/

/******************************************************************************************************************
 * @fn      void reproducir_audio( uint8_t id_audio )
 * @brief   Añade audio a la cola de reproduccion
 * @param:  id_audio:
 * @return: void
 *****************************************************************************************************************/
void reproducir_audio( uint8_t id_audio )
{
	if( audio_struct_inicializado == 0 )
	{
		audio_struct_formatter();
		audio_struct_inicializado = 1;
	}
	if( playlist_cantidad == PLAYLIST_SIZE )	//cola llena
		detener_audio();

	playlist[ ( playlist_start + playlist_cantidad ) % PLAYLIST_SIZE ] = &( audios[id_audio] );
	playlist_cantidad++;
}

/******************************************************************************************************************
 * @fn      void detener_audio( void )
 * @brief   detiene e interrumpe el audio
 * @param:  void
 * @return: void
 *****************************************************************************************************************/
void detener_audio( void )
{
	playlist_cantidad = 0;
	muestra_actual    = 0;
}



