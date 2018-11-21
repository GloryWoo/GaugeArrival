#ifndef SVCIPC_API_H_
#define SVCIPC_API_H_
/*****************************************************************
* Project Harman Car Multimedia System
* (c) copyright 2009
* Company Harman/Becker Automotive Systems GmbH
* All rights reserved
* Secrecy Level STRICTLY CONFIDENTIAL
****************************************************************/
/**
* @file svcipc_api.h
* @ingroup svcipc
* @author Glenn Schmottlach
* Public API for the service IPC library.
*/


#ifdef __cplusplus
extern "C" {
#endif

#include "svcipc/svcipc_export.h"
#include "svcipc/svcipc_types.h"
#include "svcipc/svcipc_error.h"

/**
 * @brief Initializes the service IPC library.
 *
 * This function must be called (at least) once to initialize the IPC library.
 * Subsequent calls will be ignored.
 *
 * @returns Test return value for error with SVCIPC_IS_ERROR() macro.
 *
 * Re-entrant: Yes
 * ThreadSafe: No
 */
SVCIPC_API SVCIPC_tError SVCIPC_initialize(void);


/**
 * @brief Uninitializes the IPC library and frees any allocated resources.
 *
 * This function de-allocates any resources used by the IPC library. Once
 * this function is called no other IPC functions should be called except
 * SVCIPC_initialize().
 *
 * @returns None
 *
 * Re-entrant: Yes
 * ThreadSafe: No
 */
SVCIPC_API void SVCIPC_shutdown(void);


/**
 * @brief Asynchronously opens a (potentially private) connection to a
 *        message bus.
 *
 * Opens a connection to the designated message bus at the specified
 * address. This function will also attempt register with the message bus and
 * if successful, receive a unique name. The status (success/failure) of
 * opening the connection is returned via the callback. If successfull then
 * the connection is returned as well.
 *
 * @param address The address of the IPC server.
 * @param openPrivate Indicator of whether to open a private connection (true)
 *                    or not (false).
 * @param onConnect The callback function to invoke when the operation is
 *                  complete (either success or failure). This must be
 *                  specified (e.g. it cannot be NULL).
 * @param token A user defined token to be returned with the callback.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of the request is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncOpenConnection(SVCIPC_tConstStr address,
                                          SVCIPC_tBool openPrivate,
                                          SVCIPC_tConnectionCallback onConnect,
                                          SVCIPC_tUserToken token);


/**
 * @brief Synchronously opens a (potentially private) connection to a
 *        message bus.
 *
 * Opens a connection to the designated message bus at the specified
 * address. This function will also attempt register with the message bus and
 * if successful, receive a unique name. The status (success/failure) of
 * getting the connection is indicated via the return code.
 *
 * @param address The address of the IPC server.
 * @param openPrivate Indicator of whether to open a private connection (true)
 *                    or not (false).
 * @param conn A pointer to a SVCIPC_tConnection which, on success, is
 *             set to the connection.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_openConnection(SVCIPC_tConstStr address,
                                               SVCIPC_tBool openPrivate,
                                               SVCIPC_tConnection* conn);

/**
 * @brief Asynchronously opens a (potentially private) well-known message bus.
 *
 * Opens a connection to the designated well-known message bus. This function
 * will also register with the message bus and if successful, receive a unique
 * name. The status (success/failure) of getting the connection is returned
 * via the callback. If successfull then the connection is returned as well.
 *
 * @param connType The type of well-known message bus connection to establish.
 * @param openPrivate Indicator of whether to open a private connection (true)
 *                    or not (false).
 * @param onConnect The callback function to invoke when the operation has
 *                  completed. This must be specified (e.g. it cannot be NULL).
 * @param token A user defined token to be returned with the callback.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of the request is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncGetConnection(SVCIPC_tConnType connType,
                                         SVCIPC_tBool openPrivate,
                                         SVCIPC_tConnectionCallback onConnect,
                                         SVCIPC_tUserToken token);


/**
 * @brief Synchronously opens a (potentially private) well-known message bus.
 *        The function will block while the connection is retrieved.
 *
 * Opens a connection to the designated well-known message bus. This function
 * will also register with the message bus and if successful, receive a unique
 * name. The status (success/failure) of getting the connection is indicated
 * via the return code.
 *
 * @param connType The type of well-known message bus connection to establish.
 * @param openPrivate Indicator of whether to open a private connection (true)
 *                    or not (false).
 * @param conn A pointer to a SVCIPC_tConnection which, on success, is
 *             set to the connection.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_getConnection(SVCIPC_tConnType connType,
                                              SVCIPC_tBool openPrivate,
                                              SVCIPC_tConnection* conn);


/**
 * @brief Synchronously closes a connection.
 *
 * This function closes a private connection or unreferences a shared
 * message bus connection. Once closed this connection should no longer
 * be used.
 *
 * @param conn The connection to close (if private) or unreference if
 *             shared.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          close request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_closeConnection(SVCIPC_tConnection conn);


/**
 * @brief Asynchronously invokes a method on a service.
 *
 * This function is used to asynchronously invoke a method on a specific object
 * on the designated bus.
 *
 * @param conn The connection on which to invoke the method.
 * @param busName The bus name where the method should be directed. This
 *                must not be NULL.
 * @param objPath The object path that will receive the request. If this
 *                parameter is NULL then the default object path will
 *                be used based on the bus name (replacing '.' with '/'
 *                and '-' with '_' and starting with '/').
 * @param method  The method to call.
 * @param parameters JSON encoded parameters associated with the method request.
 *                   If NULL is specified then the library will substitute an
 *                   JSON expression for an empty object '{}'.
 * @param noReplyExpected This flag, when TRUE, provides a *hint* to the
 *                        the service receiving the request that the client
 *                        doesn't care about the result. As an optimization
 *                        step the service implementing the request can skip
 *                        sending a reply (or error). Since this is only
 *                        a *hint* to the service, it can choose to ignore
 *                        it and still provide a reply (or error).
 * @param msecTimeout The time to wait (in milliseconds) for a reply to the
 *                    request. If no reply is received then a timeout will
 *                    occur and suitable error returned. If the timeout is set
 *                    to -1 then a default timeout value will be used.
 *                    Although there is no way to wait indefinitely a value of
 *                    INT_MAX should be specified which will be clamped to
 *                    several hours by the underlying library.
 * @param onResult This callback function that will be invoked when the
 *                 operation is complete. This may include a JSON encoded
 *                 result if the request is successfull. If this is NULL then
 *                 no result or error will be delivered (and thus the reply
 *                 from the service is effectively ignored).
 * @param handle A pointer to a variable that will be filled in with the
 *               handle associated with this method invocation. If NULL
 *               is passed in then the handle will NOT be set.
 * @param token A user defined token to be returned with the callback.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of the request is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncInvoke(SVCIPC_tConnection conn,
                                            SVCIPC_tConstStr busName,
                                            SVCIPC_tConstStr objPath,
                                            SVCIPC_tConstStr method,
                                            SVCIPC_tConstStr parameters,
                                            SVCIPC_tBool noReplyExpected,
                                            SVCIPC_tUInt32 msecTimeout,
                                            SVCIPC_tResultCallback onResult,
                                            SVCIPC_tHandle* handle,
                                            SVCIPC_tUserToken token);


/**
 * @brief Synchronously invokes a method on a service.
 *
 * This function is used to invoke a method on a specific object on the
 * designated bus and wait for a reply.
 *
 * @param conn The connection on which to invoke the method.
 * @param busName The bus name where the method should be directed. This
 *                must not be NULL.
 * @param objPath The object path that will receive the request. If this
 *                parameter is NULL then the default object path will be used
 *                based on the bus name (replacing '.' with '/' and '-' with
 *                '_' and starting with '/').
 * @param method  The method to call.
 * @param parameters JSON encoded parameters associated with the method request.
 *                   If NULL is specified then the library will substitute an
 *                   JSON expression for an empty object '{}'.
 * @param msecTimeout The time to wait (in milliseconds) for a reply to the
 *                    request. If no reply is received then a timeout will
 *                    occur and suitable error returned. If the timeout is set
 *                    to -1 then a default timeout value will be used.
 *                    Although there is no way to wait indefinitely a value of
 *                    INT_MAX should be specified which will be clamped to
 *                    several hours by the underlying library.
 * @param response A pointer to a response structure containing the status
 *                 (success/failure) of the invocation and the results
 *                 returned from the request. The structure should be freed
 *                 with a call to SVCIPC_freeResponse().
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_invoke(SVCIPC_tConnection conn,
                                       SVCIPC_tConstStr busName,
                                       SVCIPC_tConstStr objPath,
                                       SVCIPC_tConstStr method,
                                       SVCIPC_tConstStr parameters,
                                       SVCIPC_tUInt32 msecTimeout,
                                       SVCIPC_tResponse** response);


/**
 * @brief Frees the resources associated with a synchronous response.
 *
 * This function should be called to free the resources associated with
 * the response returned from the synchronous SVCIPC_invoke() function.
 *
 * @param response The response structure to free.
 *
 * @returns None
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API void SVCIPC_freeResponse(SVCIPC_tResponse* response);


/**
 * @brief Synchronously attempts to cancel a pending method invocation.
 *
 * This function makes a best effort attempt to cancel a pending method
 * invocation. A method will not be cancelled if it was never sent to the
 * service implementing the method or if the reply has already been received.
 *
 * @param handle The handle associated with the method invocation.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          cancel request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_cancel(SVCIPC_tHandle handle);


/**
 * @brief Asynchronously emits a signal.
 *
 * This function is used to emit a signal from a specific object on the
 * designated bus.
 *
 * @param regHnd The handle for the registered service from which this
 *               signal will be emitted.
 * @param sigName The name of the signal to emit.
 * @param parameters JSON encoded parameters associated with the signal.
 *                   If NULL is specified then the library will substitute an
 *                   JSON expression for an empty object '{}'.
 * @param onStatus This callback function is invoked when the operation is
 *                 complete.
 * @param token A user defined token to be returned with the callbacks.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of the operation is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncEmit(SVCIPC_tSvcRegHnd regHnd,
                                          SVCIPC_tConstStr sigName,
                                          SVCIPC_tConstStr parameters,
                                          SVCIPC_tStatusCallback onStatus,
                                          SVCIPC_tUserToken token);


/**
 * @brief Synchronously emits a signal.
 *
 * This function is used to synchronously emit a signal from a specific object
 * on the designated bus. This function will block until the internal
 * dispatch thread actually emits the signal.
 *
 * @param regHnd The handle for the registered service from which this
 *               signal will be emitted.
 * @param sigName The name of the signal to emit.
 * @param parameters JSON encoded parameters associated with the signal.
 *                   If NULL is specified then the library will substitute an
 *                   JSON expression for an empty object '{}'.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_emit(SVCIPC_tSvcRegHnd regHnd,
                                     SVCIPC_tConstStr sigName,
                                     SVCIPC_tConstStr parameters);

/**
 * @brief Asynchronously subscribes to a signal on a service.
 *
 * This function is used to subscribe to a signal on a specific object on the
 * designated bus.
 *
 * @param conn The connection on which to subscribe to a signal.
 * @param objPath The path to the object emitting the signal. This must
 *                be a unique object path in order to disambiguate signals
 *                sent from one bus from another.
 * @param sigName The name of the signal.
 * @param onSignal This callback function will be invoked if the specified
 *                 signal is received.
 * @param onSubscription The callback function that will be invoked if the
 *                       subscription operation completes (either successfully
 *                       or unsuccessfully). If successfull then a
 *                       subscription handle is returned which is needed
 *                       for unsubscribing from the signal.
 * @param token A user defined token to be returned with the callbacks.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of the request is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncSubscribe(SVCIPC_tConnection conn,
                                  SVCIPC_tConstStr objPath,
                                  SVCIPC_tConstStr sigName,
                                  SVCIPC_tSignalCallback onSignal,
                                  SVCIPC_tSubscriptionCallback onSubscription,
                                  SVCIPC_tUserToken token);


/**
 * @brief Synchronously subscribes to a signal on a service.
 *
 * This function is used to synchronously subscribe to a signal on a specific
 * object on the designated bus.
 *
 * @param conn The connection on which to subscribe to a signal.
 * @param objPath The path to the object emitting the signal. This must
 *                be a unique object path in order to disambiguate signals
 *                sent from one bus from another.
 * @param sigName The name of the signal.
 * @param onSignal This callback function will be invoked if the specified
 *                 signal is received.
 * @param token A user defined token to be returned with the callback.
 * @param subHnd A pointer to a SVCIPC_tSigSubHnd which, on success, is
 *               set to the subscription handle.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_subscribe(SVCIPC_tConnection conn,
                                          SVCIPC_tConstStr objPath,
                                          SVCIPC_tConstStr sigName,
                                          SVCIPC_tSignalCallback onSignal,
                                          SVCIPC_tUserToken token,
                                          SVCIPC_tSigSubHnd* subHnd);


/**
 * @brief Asynchronously unsubscribes to a signal on a service.
 *
 * This function is used to unsubscribe to a signal on a specific object on the
 * designated bus.
 *
 * @param subHnd The signal subscription handle created by the call
 *               to subscribe to the given signal.
 * @param onStatus The callback function that will be invoked if the
 *                 unsubscription completes (either successfully or
 *                 unsuccessfully).
 * @param token A user defined token to be returned with the callbacks.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of the operation is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncUnsubscribe(SVCIPC_tSigSubHnd subHnd,
                                             SVCIPC_tStatusCallback onStatus,
                                             SVCIPC_tUserToken token);


/**
 * @brief Synchronously unsubscribes to a signal on a service.
 *
 * This function is used to synchronously unsubscribe to a signal on a
 * specific object on the designated bus.
 *
 * @param subHnd The signal subscription handle created by the call
 *               to subscribe to the given signal.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_unsubscribe(SVCIPC_tSigSubHnd subHnd);

/**
 * @brief Asynchronously registers a service on the message bus.
 *
 * This function is used to register a service (and object instance) on
 * the message bus.
 *
 * @param conn The connection on which to register the service.
 * @param busName The bus name associated with the service.
 * @param objPath The path to the object implementing the service. If this
 *                parameter is NULL or empty then the default object path
 *                will be used. The default object path is synthesized from
 *                the bus name. This is created by prefixing the bus name
 *                with '/' and then substituting '-' with '_' and substituting
 *                '.' with '/'.
 * @param flag Reserved for the future. Set to 0 for now.
 * @param onRequest This callback function that will receive requests from
 *                  clients of the service.
 * @param onRegister The callback function that will be invoked if the
 *                   registration completes (with success or failure). If
 *                   successfull the result will include a valid registration
 *                   handle that needs to be provided when unregistering
 *                   the service.
 * @param token A user defined token to be returned with the callbacks.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of the request is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncRegisterService(SVCIPC_tConnection conn,
                                        SVCIPC_tConstStr busName,
                                        SVCIPC_tConstStr objPath,
                                        SVCIPC_tUInt32 flag,
                                        SVCIPC_tRequestCallback onRequest,
                                        SVCIPC_tRegistrationCallback onRegister,
                                        SVCIPC_tUserToken token);



/**
 * @brief Synchronously registers a service on the message bus.
 *
 * This function is used to synchronously register a service (and object
 * instance) on the message bus.
 *
 * @param conn The connection on which to register the service.
 * @param busName The bus name associated with the service.
 * @param objPath The path to the object implementing the service. If this
 *                parameter is NULL or empty then the default object path
 *                will be used. The default object path is synthesized from
 *                the bus name. This is created by prefixing the bus name
 *                with '/' and then substituting '-' with '_' and substituting
 *                '.' with '/'.
 * @param flag Reserved for the future. Set to 0 for now.
 * @param onRequest This callback function that will receive requests from
 *                  clients of the service.
 * @param token A user defined token to be returned with the callback.
 * @param regHnd A pointer to a registration handle that will be initialized
 *               if the registration is successfull.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_registerService(SVCIPC_tConnection conn,
                                              SVCIPC_tConstStr busName,
                                              SVCIPC_tConstStr objPath,
                                              SVCIPC_tUInt32 flag,
                                              SVCIPC_tRequestCallback onRequest,
                                              SVCIPC_tUserToken token,
                                              SVCIPC_tSvcRegHnd* regHnd);


/**
 * @brief Asynchronously unregisters a service from the message bus.
 *
 * This function is used to unregister a service from the message bus.
 *
 * @param regHnd The service registration handle obtained when the service
 *               was registered.
 * @param onSuccess This callback function that will be invoked if
 *                  unregistration completes.
 * @param token A user defined token to be returned with the callbacks.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of the operation is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncUnregisterService(SVCIPC_tSvcRegHnd regHnd,
                                             SVCIPC_tStatusCallback onStatus,
                                             SVCIPC_tUserToken token);


/**
 * @brief Synchronously unregisters a service from the message bus.
 *
 * This function is used to synchronously unregister a service from the
 * message bus.
 *
 * @param regHnd The service registration handle obtained when the service
 *               was registered.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_unregisterService(SVCIPC_tSvcRegHnd regHnd);


/**
 * @brief Asynchronously provides a mechanism to return the result of a
 *        service request.
 *
 * This function, coupled with the requesting context, provides a mechanism
 * to return a JSON encoded result string to the client who made the request.
 *
 * @param context The context of the request that will be associated with
 *                the reply. Once this function is called the context data
 *                is no longer valid and should be freed with a call to
 *                SVCIPC_freeReqContext().
 * @param result A JSON encoded result to be delivered to the requestor.
 *               If NULL is specified then the library will substitute an
 *               JSON expression for an empty object '{}'.
 * @param onStatus This callback function that will be invoked when the
 *                 result is returned. If this parameter is NULL then no
 *                 callback is invoked.
 * @param token A user defined token to be returned with the callbacks.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of this operation is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncReturnResult(SVCIPC_tReqContext context,
                                              SVCIPC_tConstStr result,
                                              SVCIPC_tStatusCallback onStatus,
                                              SVCIPC_tUserToken token);


/**
 * @brief Provides a mechanism to synchronously return the result of a
 *        service request.
 *
 * This function, coupled with the requesting context, provides a mechanism
 * to return a JSON encoded result string synchronously to the client who made
 * the request.
 *
 * @param context The context of the request that will be associated with
 *                the reply. Once this function is called the context data
 *                is no longer valid and should be freed with a call to
 *                SVCIPC_freeReqContext().
 * @param result A JSON encoded result to be delivered to the requestor.
 *               If NULL is specified then the library will substitute an
 *               JSON expression for an empty object '{}'.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_returnResult(SVCIPC_tReqContext context,
                                             SVCIPC_tConstStr result);

/**
 * @brief Asynchronously provides a mechanism to return an error to a service
 *        request.
 *
 * This function, coupled with the requesting context, provides a mechanism
 * to return an error message in response to a request.
 *
 * @param context The context of the request that will be associated with
 *                the error reply. Once this function is called the
 *                context data is no longer valid and should be freed
 *                with a call to SVCIPC_freeReqContext().
 * @param name The name for the error. This should follow the same naming
 *             convention (and constraints) as a D-Bus bus name.
 *             For example, this *should* be formatted as follows:
 *                      com.harman.service.Error
 *             If the name is not specified (e.g. NULL) then the default
 *             value is used (com.harman.service.Error).
 * @param msg A short message describing the error. If JSON messages are
 *            being pass then the message *should* contain a JSON object
 *            with optional error parameters. If NULL is specified then the
 *            library will substitute an JSON expression for an empty
 *            object '{}'.
 * @param onStatus This callback function that will be invoked when the
 *                 error is returned. If this parameter is NULL then no
 *                 callback is invoked.
 * @param token A user defined token to be returned with the callbacks.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of this operation is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncReturnError(SVCIPC_tReqContext context,
                                      SVCIPC_tConstStr name,
                                      SVCIPC_tConstStr msg,
                                      SVCIPC_tStatusCallback onStatus,
                                      SVCIPC_tUserToken token);


/**
 * @brief Provides a mechanism to synchronously return an error to a
 *        service request.
 *
 * This function, coupled with the requesting context, provides a mechanism
 * to synchronously return an error message in response to a request.
 *
 * @param context The context of the request that will be associated with
 *                the error reply. Once this function is called the
 *                context data is no longer valid and should be freed
 *                with a call to SVCIPC_freeReqContext().
 * @param name The name for the error. This should follow the same naming
 *             convention (and constraints) as a D-Bus bus name.
 *             For example, this *should* be formatted as follows:
 *                      com.harman.service.Error
 *             If the name is not specified (e.g. NULL) then the default
 *             value is used (com.harman.service.Error).
 * @param msg A short message describing the error. If JSON messages are
 *            being pass then the message *should* contain a JSON object
 *            with optional error parameters. If NULL is specified then the
 *            library will substitute an JSON expression for an empty
 *            object '{}'.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_returnError(SVCIPC_tReqContext context,
                                            SVCIPC_tConstStr name,
                                            SVCIPC_tConstStr msg);

/**
 * @brief Frees the resources associated with request context.
 *
 * This function can be called to deliberately free the resources associated
 * with the request context returned when a service request is received by
 * a server. This should ALWAYS be called after calling SVCIPC_*ReturnResult()
 * or SVCIPC_*ReturnError() to free the context.
 *
 * @param context The request context to free.
 *
 * @returns None
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API void SVCIPC_freeReqContext(SVCIPC_tReqContext context);


/**
 * @brief Provides a mechanism to synchronously determine whether an
 *        advertised bus name has an owner and is thus present.
 *
 * This function can be used to synchronously detect whether or not
 * a service with the given bus name exists and has an owner.
 *
 * @param conn The connection to use to determine whether the bus name
 *             has an owner.
 * @param busName The bus name to check to see if there is an owner.
 * @param hasOwner A pointer to a boolean that is set to 'true' if the
 *                 bus name is owned or 'false' otherwise.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value. If the bus name has an
 *          owner then the 'hasOwner' parameter will be true, otherwise
 *          it will be false.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_nameHasOwner(SVCIPC_tConnection conn,
                                             SVCIPC_tConstStr busName,
                                             SVCIPC_tBool* hasOwner);


/**
 * @brief Provides a mechanism to asynchronously determine whether an
 *        advertised bus name has an owner and is thus present.
 *
 * This function can be used to asynchronously detect whether or not
 * a service with the given bus name exists and has an owner.
 *
 * @param conn The connection to use to determine whether the bus name
 *             has an owner.
 * @param busName The bus name to check to see if there is an owner.
 * @param onHasOwner A callback that is called to indicate whether or
 *                   not the bus name has an owner.
 * @param token A user defined token that will be passed back in the
 *              callback.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of this operation is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncNameHasOwner(SVCIPC_tConnection conn,
                                       SVCIPC_tConstStr busName,
                                       SVCIPC_tNameHasOwnerCallback onHasOwner,
                                       SVCIPC_tUserToken token);


/**
 * @brief Provides a mechanism to asynchronously register to receive
 *        bus name owner changed signal.
 *
 * This function can be used to register to receives signals that indicate
 * when bus names appear and disappear or change owners. It is a mechanism
 * that can be used to determine when a service becomes available (or not).
 * This is the asynchronous interface for subscribing to this signal.
 *
 * @param conn The connection on which to subscribe to this signal.
 * @param busName The bus name to monitor. If this parameter is NULL
 *                or empty then all bus names will be monitored.
 * @param onOwnerChanged This callback function is called whenever there
 *                       is an owner change.
 * @param onSubscription This callback function is called to deliver the
 *                       subscription handle and give an indication of
 *                       whether the subscription succeeded.
 * @param token A user defined token that will be passed back in the
 *              callbacks.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing the
 *          request. If there was an error enqueuing the request then
 *          the SVCIPC_IS_ERROR() macro can be used to detect it. The ultimate
 *          success/failure of this operation is conveyed in the callback.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_asyncSubscribeOwnerChanged(
                               SVCIPC_tConnection conn,
                               SVCIPC_tConstStr busName,
                               SVCIPC_tNameOwnerChangedCallback onOwnerChanged,
                               SVCIPC_tSubscriptionCallback onSubscription,
                               SVCIPC_tUserToken token);

/**
 * @brief Provides a mechanism to synchronously register to receive
 *        bus name owner changed signal.
 *
 * This function can be used to register to receives signals that indicate
 * when bus names appear and disappear or change owners. It is a mechanism
 * that can be used to determine when a service becomes available (or not).
 *
 * @param conn The connection on which to subscribe to this signal.
 * @param busName The bus name to monitor. If this parameter is NULL
 *                or empty then all bus names will be monitored.
 * @param onOwnerChanged This callback function is called whenever there
 *                       is an owner change.
 * @param token A user defined token that will be passed back in the
 *              callback.
 * @param subHnd A pointer to a handle that holds the subscription. A
 *               call to SVCIPC_unsubscribe() can be made to cancel the
 *               subscription.
 *
 * @returns Returns SVCIPC_ERROR_NONE if there is no error enqueuing and
 *          executing the request. Use the SVCIPC_IS_ERROR() macro to
 *          detect errors in the returned value. On success the subscription
 *          handle is initialized.
 *
 * Re-entrant: Yes
 * ThreadSafe: Yes
 */
SVCIPC_API SVCIPC_tError SVCIPC_subscribeOwnerChanged(SVCIPC_tConnection conn,
                      SVCIPC_tConstStr busName,
                      SVCIPC_tNameOwnerChangedCallback onOwnerChanged,
                      SVCIPC_tUserToken token,
                      SVCIPC_tSigSubHnd* subHnd);

#ifdef __cplusplus
}
#endif

#endif /* Guard for SVCIPC_API_H_*/
