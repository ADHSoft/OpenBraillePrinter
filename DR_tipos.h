
#ifndef DR_TIPOS_H_
#define DR_TIPOS_H_

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/
#define     __R					volatile const  	// !< Modificador para solo lectura para registros
#define 	__W     			volatile 	       	// !<  Modificador para solo escritura para registros
#define 	__RW				volatile           	// !< Modificador lectura / escritura para registros

#define 	ON					1
#define 	OFF					0

#define 	NULL				(( void *) 0)

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

typedef		unsigned int		uint32_t;
typedef		short unsigned int	uint16_t;
typedef		unsigned char		uint8_t ;
typedef		int					int32_t;
typedef		short int			int16_t;
typedef		char				int8_t;

typedef enum { FALSE , TRUE } bool_t;

typedef struct
{
	const uint8_t* pcm_data;	//check const
	uint32_t Size;
} audio_t;


#endif /* DR_TIPOS_H_ */
