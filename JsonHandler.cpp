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
#include "JsonHandler.hpp"
#include "TraceMacros.hpp"

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

/*-----------------------------------------------------------------------------\
 * File Scope Variables
 \*----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------\
 * Global Function Definitions
 \*----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------\
 * Class Function Definitions
 \*----------------------------------------------------------------------------*/

bool JsonHandler::ParseJsonString(const std::string& inParam,
                                  Json::Value& outRoot)
{

   bool success = true;

   if ("" == inParam)
   {
      CODE_TRACE_DBG_ERROR(("The string is invalid."));
      return false;
   }

   Json::Reader jReader;
   success = jReader.parse(inParam, outRoot, false);
   if (false == success)
   {
      CODE_TRACE_DBG_ERROR(("ERROR: Failed to parse: %s", jReader.getFormatedErrorMessages().c_str()));
   }

   return success;

}

std::string JsonHandler::CreateStyledJsonStream(const Json::Value& inJsonStream)
{

   std::string styledStream = "";

   bool isParamValid = inJsonStream.empty();
   if (isParamValid == false)
   {
      Json::StyledWriter styledWriter;

      styledStream = styledWriter.write(inJsonStream);
   }
   else
   {
      CODE_TRACE_DBG_ERROR(("Invalid JSON Input-> Received Empty..."));
   }

   return styledStream;

}

bool JsonHandler::ValidateJsonInt(const std::string& inParam,
                                  const Json::Value& inJsonValue,
                                  int& outInt)
{

   //First check if json string sent by client of this service is empty or not
   bool isParamValid = inJsonValue.empty();

   //If Not empty, then proceed
   if (isParamValid == false)
   {
      //Get the value for given in-param and check if the named param exists or not
      Json::Value jsonValue = inJsonValue.get(inParam.c_str(),
                                              Json::Value(Json::nullValue));

      if (jsonValue != Json::nullValue)
      {
         //Check if the "data type" of the in-param is as per expectations
         //**** Checking for isInt() because if we use isUInt() it fails, this seems to be a bug in JsonCpp library ***
         isParamValid = jsonValue.isInt();
         if (true == isParamValid)
         {
            //Now get the actual value for the in-param
            outInt = jsonValue.asInt();
         } //if(true == isParamValid)
         else
         {
            CODE_TRACE_DBG_ERROR(("Invalid JSON Input -> Expected UInt..."));
         }
      } //if(jsonValue != Json::nullValue)
   } //if(isParamValid == false)
   else
   {
      CODE_TRACE_DBG_ERROR(("Invalid JSON Input-> Received Empty..."));
   }

   return isParamValid;

}

bool JsonHandler::ValidateJsonDouble(const std::string& inParam,
                                     const Json::Value& inJsonValue,
                                     double& outInt)
{

   //First check if json string sent by client of this service is empty or not
   bool isParamValid = inJsonValue.empty();

   //If Not empty, then proceed
   if (isParamValid == false)
   {
      //Get the value for given in-param and check if the named param exists or not
      Json::Value jsonValue = inJsonValue.get(inParam.c_str(),
                                              Json::Value(Json::nullValue));

      if (jsonValue != Json::nullValue)
      {
         //Check if the "data type" of the in-param is as per expectations
         //**** Checking for isInt() because if we use isUInt() it fails, this seems to be a bug in JsonCpp library ***
         isParamValid = jsonValue.isDouble();
         if (true == isParamValid)
         {
            //Now get the actual value for the in-param
            outInt = jsonValue.asDouble();
         } //if(true == isParamValid)
         else
         {
            CODE_TRACE_DBG_ERROR(("Invalid JSON Input -> Expected UInt..."));
         }
      } //if(jsonValue != Json::nullValue)
   } //if(isParamValid == false)
   else
   {
      CODE_TRACE_DBG_ERROR(("Invalid JSON Input-> Received Empty..."));
   }

   return isParamValid;

}

bool JsonHandler::ValidateJsonBool(const std::string& inParam,
                                   const Json::Value& inJsonValue,
                                   bool& outBool)
{
   //First check if json string sent by client of this service is empty or not

   bool isParamValid = inJsonValue.empty();

   //If Not empty, then proceed
   if (isParamValid == false)
   {
      //Get the value for given in-param and check if the named param exists or not
      Json::Value jsonValue = inJsonValue.get(inParam.c_str(),
                                              Json::Value(Json::nullValue));

      if (jsonValue != Json::nullValue)
      {
         //Check if the "data type" of the in-param is as per expectations
         isParamValid = jsonValue.isBool();
         if (true == isParamValid)
         {
            //Now get the actual value for the in-param
            outBool = jsonValue.asBool();
         } //if(true == isParamValid)
         else
         {
            CODE_TRACE_DBG_ERROR(("Invalid JSON Input -> Expected Bool..."));
         }
      } //if(jsonValue != Json::nullValue)
   } //if(isParamValid == false)
   else
   {
      CODE_TRACE_DBG_ERROR(("Invalid JSON Input-> Received Empty..."));
   }

   return isParamValid;

}

bool JsonHandler::ValidateJsonString(const std::string& inParam,
                                     const Json::Value& inJsonValue,
                                     std::string& outString)
{
   //First check if json string sent by client of this service is empty or not

   bool isParamValid = inJsonValue.empty();

   //If Not empty, then proceed
   if (isParamValid == false)
   {
      //Get the value for given in-param and check if the named param exists or not
      Json::Value jsonValue = inJsonValue.get(inParam.c_str(),
                                              Json::Value(Json::nullValue));

      if (jsonValue != Json::nullValue)
      {
         //Check if the "data type" of the in-param is as per expectations
         isParamValid = jsonValue.isString();
         if (true == isParamValid)
         {
            //Now get the actual value for the in-param
            outString = jsonValue.asString();
         } //if(true == isParamValid)
         else
         {
            CODE_TRACE_DBG_ERROR(("Invalid JSON Input -> Expected String..."));
         }
      } //if(jsonValue != Json::nullValue)
   } //if(isParamValid == false)
   else
   {
      CODE_TRACE_DBG_ERROR(("Invalid JSON Input-> Received Empty..."));
   }

   return isParamValid;

}

bool JsonHandler::ValidateJsonObject(const std::string& inParam,
                                     const Json::Value& inJsonValue,
                                     Json::Value& outJsonValue)
{

   //First check if json string sent by client of this service is empty or not
   bool isParamValid = inJsonValue.empty();

   //If Not empty, then proceed
   if (isParamValid == false)
   {
      //Get the value for given in-param and check if the named param exists or not
      Json::Value jsonValue = inJsonValue.get(inParam.c_str(),
                                              Json::Value(Json::nullValue));

      if (jsonValue != Json::nullValue)
      {
         //Check if the "data type" of the in-param is as per expectations
         isParamValid = jsonValue.isObject();
         if (true == isParamValid)
         {
            //Now get the actual value for the in-param
            outJsonValue = jsonValue;
         } //if(true == isParamValid)
         else
         {
            CODE_TRACE_DBG_ERROR(("Not JSON Object."));
         }
      } //if(jsonValue != Json::nullValue)
   } //if(isParamValid == false)
   else
   {
      CODE_TRACE_DBG_ERROR(("Invalid JSON Input-> Received Empty..."));
   }

   return isParamValid;

}

bool JsonHandler::ValidateJsonArray(const std::string& inParam,
                                    const Json::Value& inJsonValue)
{
   //First check if json string sent by client of this service is empty or not
   bool isParamValid = inJsonValue.empty();

   //If Not empty, then proceed
   if (isParamValid == false)
   {
      //Get the value for given in-param and check if the named param exists or not
      Json::Value jsonValue = inJsonValue.get(inParam.c_str(),
                                              Json::Value(Json::nullValue));

      if (jsonValue != Json::nullValue)
      {
         //Check if the "data type" of the in-param is as per expectations
         isParamValid = jsonValue.isArray();
      } //if(jsonValue != Json::nullValue)
      else
      {
         CODE_TRACE_DBG_ERROR(("Invalid JSON Input -> Expected Json:Array..."));
      }
   } //if(isParamValid == false)
   else
   {
      CODE_TRACE_DBG_ERROR(("Invalid JSON Input-> Received Empty..."));
   }

   return isParamValid;

}

/*--------------------------------- EOF --------------------------------------*/
