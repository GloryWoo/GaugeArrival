#ifndef CODE_TRACE_MACROS_HPP
#define CODE_TRACE_MACROS_HPP

/*******************************************************************************
 *  Project       Geofence
 *  (c) copyright 2013
 *  Company       Harman International
 *                All rights reserved
 * Secrecy Level  STRICTLY CONFIDENTIAL
 ******************************************************************************/

/**
 *  @file           TraceMacros.hpp
 *  @brief          This class defines the trace macros.
 *  @authors      Shanghai, CTG
 *                      Saiya LIU
 *  @ingroup      Geofence Subsystem
 */

/*-----------------------------------------------------------------------------\
 * Preprocessor Includes
 \*----------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

/*-----------------------------------------------------------------------------\
 * Preprocessor Defines (Macros, Constants)
 \*----------------------------------------------------------------------------*/

// error information will always be displayed.
#define CODE_TRACE_DBG_ERROR(a) do { \
   const char *pFileName = strrchr(__FILE__, '/'); \
   if (!pFileName) \
   { \
      pFileName = __FILE__; \
   } \
   else \
   { \
     pFileName++; \
   } \
   printf("Error: %s: %s(): line %d: ", pFileName, __FUNCTION__, __LINE__); \
   printf a; \
   printf("\n"); \
} while(0)

#define CODE_TRACE_CHECK_COND(cond) do { \
  if (!(cond))\
  {\
    const char *pFileName = strrchr(__FILE__, '/');\
    if (!pFileName)\
    {\
      pFileName = __FILE__;\
    }\
    else\
    {\
      pFileName++;\
    }\
    printf("%s, null pointer: %s: %s(): line %d: \n",#cond, pFileName, __FUNCTION__, __LINE__);\
  }\
}while(0)

#ifdef DEBUG_TAG
#define CODE_TRACE_ENTER_METHOD() do { \
   printf("ENTER: "); \
   const char *pFileName = strrchr(__FILE__, '/'); \
   if (!pFileName) \
   { \
      pFileName = __FILE__; \
   } \
   else \
   { \
     pFileName++; \
   } \
   printf("%s: %s().\n", pFileName, __FUNCTION__); \
} while(0)

#define CODE_TRACE_EXIT_METHOD() do { \
   printf("EXIT : "); \
   const char *pFileName = strrchr(__FILE__, '/'); \
   if (!pFileName) \
   { \
      pFileName = __FILE__; \
   } \
   else \
   { \
     pFileName++; \
   } \
   printf("%s: %s().\n", pFileName, __FUNCTION__); \
} while(0)

#define CODE_TRACE_DBG_MSG(a) do { \
   const char *pFileName = strrchr(__FILE__, '/'); \
   if (!pFileName) \
   { \
      pFileName = __FILE__; \
   } \
   else \
   { \
     pFileName++; \
   } \
   printf("MSG  : %s: %s(): line %d: ", pFileName, __FUNCTION__, __LINE__); \
   printf a; \
   printf("\n"); \
} while(0)

#define CODE_TRACE_DBG_WARN(a) do { \
   if (!pFileName) \
   { \
      pFileName = __FILE__; \
   } \
   else \
   { \
     pFileName++; \
   } \
   printf("WARN : %s: %s(): line %d: ", pFileName, __FUNCTION__, __LINE__); \
   printf a; \
   printf("\n"); \
} while(0)

#else
#define CODE_TRACE_ENTER_METHOD()
#define CODE_TRACE_EXIT_METHOD()
#define CODE_TRACE_DBG_MSG(a)
#define CODE_TRACE_DBG_WARNING(a)
#endif /* DEBUG_TAG */

/*-----------------------------------------------------------------------------\
 * Type Definitions (Enums, Typedefs, Structs, ...)
 \*----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------\
 * Constant And External Variables
 \*----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------\
 * Function Prototypes
 \*----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------\
 * Class Declarations
 \*----------------------------------------------------------------------------*/

#endif /* CODE_TRACE_MACROS_HPP */

/*--------------------------------- EOF --------------------------------------*/
