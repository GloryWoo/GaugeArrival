#ifndef SVCIPC_TYPES_H_
#define SVCIPC_TYPES_H_
/*****************************************************************
* Project Harman Car Multimedia System
* (c) copyright 2009
* Company Harman/Becker Automotive Systems GmbH
* All rights reserved
* Secrecy Level STRICTLY CONFIDENTIAL
****************************************************************/
/**
* @file svcipc_types.h
* @ingroup svcipc
* @author Glenn Schmottlach
* Public types, definitions, and enumerations of service API.
*/

#ifdef __cplusplus
extern "C" {
#endif


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

// Basic integer types
typedef unsigned char      uint8_t;
typedef short              int16_t;
typedef unsigned short     uint16_t;
typedef unsigned long      uint32_t;
typedef long               int32_t;
typedef __int64            int64_t;
typedef unsigned __int64   uint64_t;

#elif defined(__QNX__)
#include <stdint.h>
#endif

/**
 * @brief Fundamental type definitions
 */
typedef char                        SVCIPC_tChar;
typedef SVCIPC_tChar*               SVCIPC_tString;
typedef const SVCIPC_tChar*         SVCIPC_tConstStr;
typedef uint32_t                    SVCIPC_tUInt32;
typedef int32_t                     SVCIPC_tInt32;
typedef int32_t                     SVCIPC_tBool;
typedef void*                       SVCIPC_tUserToken;
typedef void*                       SVCIPC_tConnection;
typedef void*                       SVCIPC_tReqContext;
typedef void*                       SVCIPC_tSvcRegHnd;
typedef void*                       SVCIPC_tSigSubHnd;
typedef uint32_t                    SVCIPC_tError;
typedef uint32_t                    SVCIPC_tHandle;

/**
 * @brief Define boolean values for library
 */
#define SVCIPC_TRUE     (1)
#define SVCIPC_FALSE    (!SVCIPC_TRUE)


/**
 * @brief Callback status returned with all callback
 */
typedef struct SVCIPC_tCallbackStatus
{
   SVCIPC_tError     errCode;
   SVCIPC_tConstStr  errName; /* The name of the error (can be NULL */
   SVCIPC_tConstStr  errMsg;  /* An error message (can be NULL) */
} SVCIPC_tCallbackStatus;


/*
 * @brief Define the reply returned from a call to block and wait
 *        for the response from a method invocation.
 */
typedef struct SCVIPC_tResponse
{
   // This structure is identical to SVCIPC_tCallbackStatus but must
   // use non-const pointers so that the resources can be freed.
   struct SVCIPC_tResponseStatus
   {
      SVCIPC_tError   errCode;
      SVCIPC_tString  errName; /* The name of the error (can be NULL */
      SVCIPC_tString  errMsg;  /* An error message (can be NULL) */
   } status;
   SVCIPC_tString     result;
} SVCIPC_tResponse;

/**
 * @brief Define the basic callback types
 */

/* @brief Called to provide the status of an operation */
typedef void (*SVCIPC_tStatusCallback)(const SVCIPC_tCallbackStatus* status,
                                       SVCIPC_tUserToken token);

/* @brief Called to return a newly allocated connection */
typedef void (*SVCIPC_tConnectionCallback)(const SVCIPC_tCallbackStatus* status,
                                      SVCIPC_tConnection conn,
                                      SVCIPC_tUserToken token);

/* @brief Called to return the result from a service */
typedef void (*SVCIPC_tResultCallback)(const SVCIPC_tCallbackStatus* status,
                                   SVCIPC_tConstStr result,
                                   SVCIPC_tUserToken token);

/* @brief Called to deliver a request from a client */
typedef void (*SVCIPC_tRequestCallback)(SVCIPC_tReqContext context,
                                     SVCIPC_tConstStr method,
                                     SVCIPC_tConstStr parms,
                                     SVCIPC_tBool noReplyExpected,
                                     SVCIPC_tUserToken token);

/* @brief Called to deliver a signal from a service */
typedef void (*SVCIPC_tSignalCallback)(SVCIPC_tConstStr sigName,
                                     SVCIPC_tConstStr data,
                                     SVCIPC_tUserToken token);

/* @brief Called to deliver a name owner changed signal */
typedef void (*SVCIPC_tNameOwnerChangedCallback)(SVCIPC_tConstStr newName,
                                          SVCIPC_tConstStr oldOwner,
                                          SVCIPC_tConstStr newOwner,
                                          SVCIPC_tUserToken token);

/* @brief Called to deliver an indication of whether a bus name is owned */
typedef void (*SVCIPC_tNameHasOwnerCallback)(
                                    const SVCIPC_tCallbackStatus* status,
                                    SVCIPC_tConstStr busName,
                                    SVCIPC_tBool hasOwner,
                                    SVCIPC_tUserToken token);

/* @brief Called to return a newly allocated service registration */
typedef void (*SVCIPC_tRegistrationCallback)(
                                    const SVCIPC_tCallbackStatus* status,
                                    SVCIPC_tSvcRegHnd regHnd,
                                    SVCIPC_tUserToken token);

/* @brief Called to return a newly allocated signal subscription */
typedef void (*SVCIPC_tSubscriptionCallback)(
                                    const SVCIPC_tCallbackStatus* status,
                                    SVCIPC_tSigSubHnd subHnd,
                                    SVCIPC_tUserToken token);

/**
 * @brief Define the well-known message buses
 */
typedef enum
{
   SVCIPC_CONNECTION_SESSION,    /* Per-user login message bus */
   SVCIPC_CONNECTION_SYSTEM,     /* System-wide message bus */
   SVCIPC_CONNECTION_STARTER     /* Message bus that started/launched service */
} SVCIPC_tConnType;

/**
 * @brief Define an invalid handle value
 */
#define SVCIPC_INVALID_HANDLE (0U)

#ifdef __cplusplus
}
#endif

#endif /* Guard for SVCIPC_TYPES_H_ */
