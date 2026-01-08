/* File: LIB/BIT_MATH.h */

#ifndef BIT_MATH_H_
#define BIT_MATH_H_

/* I shift '1' to the bit location and use OR (|) to turn it ON (1) */
#define SET_BIT(VAR,BIT)    ((VAR) |=  (1<<(BIT)))

/* I create a mask with '0' at the bit location and use AND (&) to turn it OFF (0) */
#define CLR_BIT(VAR,BIT)    ((VAR) &= ~(1<<(BIT)))

/* I use XOR (^) to flip the bit state (if 1 becomes 0, if 0 becomes 1) */
#define TOG_BIT(VAR,BIT)    ((VAR) ^=  (1<<(BIT)))

/* I shift the target bit to the rightmost position and mask it to read its value (0 or 1) */
#define GET_BIT(VAR,BIT)    (((VAR) >> (BIT)) & 1)

/* End of the include guard */
#endif /* BIT_MATH_H_ */