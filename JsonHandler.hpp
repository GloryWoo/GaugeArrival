#ifndef JSON_HANDLER_HPP
#define JSON_HANDLER_HPP

/*******************************************************************************
 *  Project       CICC
 *  (c) copyright 20112
 *  Company       Harman International
 *                All rights reserved
 * Secrecy Level  STRICTLY CONFIDENTIAL
 ******************************************************************************/

/**
 *  @file         JsonHandler
 *  @brief        This Class is used to read(write) stream from(to) local file
 *                storage.
 *  @authors      Shanghai, CTG
 *                Jianhua Hu
 *  @ingroup      CICC Subsystem
 */

/*-----------------------------------------------------------------------------\
 * Preprocessor Includes
 \*----------------------------------------------------------------------------*/
#include <string>
#include "jsoncpp/json.h"

/*-----------------------------------------------------------------------------\
 * Preprocessor Defines (Macros, Constants)
 \*----------------------------------------------------------------------------*/

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
class JsonHandler
{
public:

   static bool
            ParseJsonString(const std::string& inParam, Json::Value& outRoot);

   static std::string CreateStyledJsonStream(const Json::Value& inJsonStream);

   static bool ValidateJsonInt(const std::string& inParam,
                               const Json::Value& inJsonValue,
                               int& outUInt);
   static bool ValidateJsonDouble(const std::string& inParam,
                                  const Json::Value& inJsonValue,
                                  double& outInt);
   static bool ValidateJsonBool(const std::string& inParam,
                                const Json::Value& inJsonValue,
                                bool& outBool);

   static bool ValidateJsonString(const std::string& inParam,
                                  const Json::Value& inJsonValue,
                                  std::string& outString);

   static bool ValidateJsonObject(const std::string& inParam,
                                  const Json::Value& inJsonValue,
                                  Json::Value& outJsonValue);

   static bool ValidateJsonArray(const std::string& inParam,
                                 const Json::Value& inJsonValue);

};

#endif /* JSON_HANDLER_HPP */
