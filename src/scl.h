
#ifndef SCL_H
#define SCL_H 1

/*
*       Simple C Library
*/

/*
*   useful macros:
*       SCL_SAFE
*       SCL_CHECK_LIMITS
*       SCL_USE_UNSIGNED_CHAR
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#ifdef SCL_SAFE
#define SCL_IS_SAFE 1
#else
#define SCL_IS_SAFE 0
#endif

#ifdef COMPILE_SCL
#define SCL_DLL_EXPORT __declspec(dllexport)
#else
#define SCL_DLL_EXPORT __declspec(dllimport)
#endif

#include "scl/check_limits.h"
#include "scl/size.h"
#include "scl/exit_codes.h"
#include "scl/printf.h"
#include "scl/error.h"
#include "scl/math.h"
#include "scl/random.h"


#ifdef SCL_USE_UNSIGNED_CHAR
#define CHAR unsigned char
#else
#define CHAR char
#endif // SCL_USE_UNSIGNED_CHAR

#include "scl/io.h"
#include "scl/string.h"

#undef CHAR


#undef SCL_DLL_EXPORT

#endif // SCL_H
