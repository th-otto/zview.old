#ifndef __2B_UNIVERSAL_TYPES__		
#define __2B_UNIVERSAL_TYPES__

typedef signed char		int8;	
typedef unsigned char	uint8;		
typedef signed long		int32;	
typedef unsigned long	uint32;	
typedef int32  			fix31;

#ifdef __GNUC__	
	typedef signed short		int16;		
	typedef unsigned short		uint16;	
	#define	__2B_HAS64_SUPPORT						/* Compiler supports 64 Bit Integers */
	typedef signed long long	int64;
	typedef unsigned long long	uint64;
#else												/* Compiler doesn't support 64 Bit Integers */
	typedef signed int			int16;		
	typedef unsigned int		uint16;	

	typedef struct
	{
		int32		hi;
		uint32		lo;
	} int64;

	typedef struct
	{
		int32		hi;
		uint32		lo;
	} loff_t;

	typedef struct
	{
		uint32	hi;
		uint32	lo;
	} uint64;
#endif


typedef int32	boolean; 


#endif																		/* __2B_UNIVERSAL_TYPES__ */
