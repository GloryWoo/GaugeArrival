#ifndef SVCIPC_EXPORT_H_
#define SVCIPC_EXPORT_H_
/*****************************************************************
* Project Harman Car Multimedia System
* (c) copyright 2009
* Company Harman/Becker Automotive Systems GmbH
* All rights reserved
* Secrecy Level STRICTLY CONFIDENTIAL
****************************************************************/
/**
* @file svcipc_export.h
* @ingroup svcipc
* @author Glenn Schmottlach
* Macros used for exporting symbols in a shared library
*/

#ifdef WIN32
#ifdef SVCIPC_EXPORTS
#define SVCIPC_API __declspec(dllexport)
#else
#define SVCIPC_API __declspec(dllimport)
#endif
#else
#define SVCIPC_API
#endif


#endif /* Guard for SVCIPC_EXPORT_H_ */
