/* File: LIB/STD_TYPES.h */

#ifndef STD_TYPES_H_
#define STD_TYPES_H_

/* I rename 'unsigned char' to 'u8' for small positive numbers (1 Byte: 0 to 255) */
typedef unsigned char        u8;

/* I rename 'signed char' to 's8' for small signed numbers (1 Byte: -128 to 127) */
typedef signed char          s8;

/* I rename 'unsigned short int' to 'u16' for medium numbers (2 Bytes: 0 to 65535) */
typedef unsigned short int   u16;

/* I rename 'signed short int' to 's16' for medium signed numbers (2 Bytes) */
typedef signed short int     s16;

/* I rename 'unsigned long int' to 'u32' for large numbers (4 Bytes) */
typedef unsigned long int    u32;

/* I rename 'signed long int' to 's32' for large signed numbers (4 Bytes) */
typedef signed long int      s32;

/* I rename 'float' to 'f32' for decimal numbers (4 Bytes) */
typedef float                f32;

/* I rename 'double' to 'f64' for precise decimal numbers (8 Bytes) */
typedef double               f64;

/* I define NULL to safely initialize pointers that point to nothing */
#define NULL                 ((void*)0)

/* End of the include guard */
#endif /* STD_TYPES_H_ */