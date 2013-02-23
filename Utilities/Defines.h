#ifndef __DEFINES_H_
#define __DEFINES_H_

/*
 *  Some compiletime configuration parameters.
 *
 *
 */


/**
 *  When define, a fast inverse squareroot method is used, which is
 *  base on Newton aproximation. This is about 3.5 times faster than
 *  1.0/sqrt(x).
 */
#define BATS_USE_FAST_INVERSE_SQRT 0

/**
 *  When define, a sse inverse squareroot method is used, which is
 *  about 2.5 times faster than even the FAST_INVERSE_SQRT! And thus
 *  8 times faster than the 1.0/sqrt(x)!!
 */
#define BATS_USE_SSE_INVERSE_SQRT 1

/**
 *  When none zero, a sse squareroot is used, which is about 2.5 times
 *  faster than the fpu version!
 */
#define BATS_USE_SSE_SQRT 1

/**
 */
#define _COUT_FLAG_CONST_LOG_ 0
#define _COUT_FLAGTYPE_POSITION_LOG_ 0

/**
 */
#define _MY_POSITION_RECTANGLE_ALL_LOG_ 0
#define _MY_POSITION_RECTANGLE_LOG_ 0
#define _MY_POSITION_RECTANGLE_WITH_F1R_AND_F2R_LOG_ 0
#define _MY_POSITION_RECTANGLE_WITH_F1L_AND_F2L_LOG_ 0
#define _MY_POSITION_RECTANGLE_WITH_G1R_AND_G2R_LOG_ 0
#define _MY_POSITION_RECTANGLE_WITH_G1L_AND_G2L_LOG_ 0

/**
 */
#if _MY_POSITION_RECTANGLE_ALL_LOG_
#define _MY_POSITION_RECTANGLE_LOG_ 0
#define _MY_POSITION_RECTANGLE_WITH_F1R_AND_F2R_LOG_ 1
#define _MY_POSITION_RECTANGLE_WITH_F1L_AND_F2L_LOG_ 1
#define _MY_POSITION_RECTANGLE_WITH_G1R_AND_G2R_LOG_ 1
#define _MY_POSITION_RECTANGLE_WITH_G1L_AND_G2L_LOG_ 1
#endif

#endif //__DEFINES_H_
