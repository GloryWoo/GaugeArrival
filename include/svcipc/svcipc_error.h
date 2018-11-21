#ifndef SVCIPC_ERROR_H_
#define SVCIPC_ERROR_H_
/*****************************************************************
* Project Harman Car Multimedia System
* (c) copyright 2009
* Company Harman/Becker Automotive Systems GmbH
* All rights reserved
* Secrecy Level STRICTLY CONFIDENTIAL
****************************************************************/
/**
* @file svcipc_error.h
* @ingroup svcipc
* @author Glenn Schmottlach
* The errors returned by the service IPC API.
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "svcipc/svcipc_export.h"
#include "svcipc/svcipc_types.h"

/*
 * @brief   Define the logical domains within the service IPC library
 */
typedef enum {
   SVCIPC_DOMAIN_IPC_LIB       = 0,
   SVCIPC_DOMAIN_DBUS_LIB      = 1,
   SVCIPC_DOMAIN_C_LIB         = 2,
   SVCIPC_DOMAIN_WIN32_LIB     = 3
} SVCIPC_tDomain;

/*
 * @brief   Define the available error levels
 */
typedef enum {
   SVCIPC_ERROR_LEVEL_NONE  = 0,
   SVCIPC_ERROR_LEVEL_WARN  = 1,
   SVCIPC_ERROR_LEVEL_ERROR = 2,
   SVCIPC_ERROR_LEVEL_FATAL = 4
} SVCIPC_tErrorLevel;

/*
 * @brief   Define the error type for the library. It's composed of a level,
 *          domain, and code.
 */

#define SVCIPC_MAKE_ERROR(level, domain, code) \
         ( (((SVCIPC_tUInt32)(level) & 0x00000003U) << 30U) | \
           (((SVCIPC_tUInt32)(domain) & 0x00000003U) << 28U) | \
           (((SVCIPC_tUInt32)(code) & 0x0FFFFFFFU)) )

#define SVCIPC_ERR_GET_LEVEL(error)   \
         (((SVCIPC_tUInt32)(error) >> 30U) & 0x00000003U)
#define SVCIPC_ERR_GET_DOMAIN(error)  \
         (((SVCIPC_tUInt32)(error) >> 28U) & 0x00000003U)
#define SVCIPC_ERR_GET_CODE(error)    \
         ((SVCIPC_tUInt32)(error) & 0x0FFFFFFFU)

#define SVCIPC_IS_ERROR(error) (0 != (SVCIPC_ERR_GET_LEVEL(error) & \
                           ((SVCIPC_tUInt32)SVCIPC_ERROR_LEVEL_ERROR | \
                           (SVCIPC_tUInt32)SVCIPC_ERROR_LEVEL_FATAL)))

/*
 * Define errors associated with the service IPC library
 */
typedef enum {
  SVCIPC_ERR_OK,
  SVCIPC_ERR_NOT_SUPPORTED,
  SVCIPC_ERR_NO_MEMORY,
  SVCIPC_ERR_BAD_ARGS,
  SVCIPC_ERR_INTERNAL,
  SVCIPC_ERR_DBUS,
  SVCIPC_ERR_CMD_SUBMISSION,
  SVCIPC_ERR_NOT_CONNECTED,
  SVCIPC_ERR_CANCELLED,
  SVCIPC_ERR_CONN_SEND,
  SVCIPC_ERR_NOT_FOUND,
  SVCIPC_ERR_DEADLOCK
} SVCIPC_tErrorCode;

extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_OK;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_NOT_SUPPORTED;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_NO_MEMORY;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_BAD_ARGS;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_INTERNAL;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_DBUS;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_CMD_SUBMISSION;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_NOT_CONNECTED;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_CANCELLED;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_CONN_SEND;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_NOT_FOUND;
extern SVCIPC_API SVCIPC_tConstStr SVCIPC_ERR_NAME_DEADLOCK;

/*
 * Convenience defintion for no errors
 */
#define SVCIPC_ERROR_NONE  (SVCIPC_MAKE_ERROR(SVCIPC_ERROR_LEVEL_NONE, \
                        SVCIPC_DOMAIN_IPC_LIB, SVCIPC_ERR_OK))

#ifdef __cplusplus
}
#endif

#endif /* Guard for SVCIPC_ERROR_H_ */
