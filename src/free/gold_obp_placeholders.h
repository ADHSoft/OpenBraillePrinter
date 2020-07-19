#ifndef FREE_FREE_OBP_PLACEHOLDERS_H_
#define FREE_FREE_OBP_PLACEHOLDERS_H_

#include "./../DR_tipos.h"

#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))


// DEFINES

#define AUDIO_PC 0
#define AUDIO_INMEDIATO 1
#define AUDIO_LISTO_PARA_IMPRIMIR 2
#define AUDIO_BUFFER 3
#define AUDIO_ERROR_CRITICO 4
#define AUDIO_HOJA_CARGADA 5
#define AUDIO_MODO 6
#define AUDIO_NO_HAY_HOJA 7
#define AUDIO_ULTIMO_RENGLON 8
#define AUDIO_SONIDO_ACTIVADO 9
#define AUDIO_SONIDO_TECLA 10
#define AUDIO_NO_HAY_MAS_MEMORIA_PARA_ESCRIBIR 11
#define AUDIO_NO_HAY_MAS_ESPACIO_EN_LA_HOJA 12
#define AUDIO_BIENVENIDO 13


// FUNCTIONS

WEAK void Nothing(void);
WEAK void detener_audio(void);
WEAK void reproducir_audio( uint8_t );
WEAK void dac_y_timer_init(void);
WEAK void InitUART0();
WEAK void UART0_PushTx( uint8_t );

void detener_audio(void) ALIAS(Nothing);
void reproducir_audio( uint8_t ) ALIAS(Nothing);
void dac_y_timer_init(void) ALIAS(Nothing);
void InitUART0(void) ALIAS(Nothing);
void UART0_PushTx( uint8_t ) ALIAS(Nothing);

int16_t UART0_PopRx( void );

void Nothing(void){
}


#endif /* FREE_FREE_OBP_PLACEHOLDERS_H_ */
