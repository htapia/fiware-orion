/*
*
* Copyright 2016 Telefonica Investigacion y Desarrollo, S.A.U
*
* This file is part of Orion Context Broker.
*
* Orion Context Broker is free software: you can redistribute it and/or
* modify it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* Orion Context Broker is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero
* General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Orion Context Broker. If not, see http://www.gnu.org/licenses/.
*
* For those usages not covered by this license please contact with
* iot_support at tid dot es
*
* Author: Fermín Galán
*/
#include <vector>

#include "mongo/client/dbclient.h"

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "parse/CompoundValueNode.h"
#include "mongoBackend/compoundValueBson.h"
#include "mongoBackend/dbFieldEncoding.h"



/* ****************************************************************************
*
* namespaces - 
*/
using namespace mongo;
using namespace orion;



/* ****************************************************************************
*
* compoundValueBson (for arrays) -
*/
void compoundValueBson(std::vector<orion::CompoundValueNode*> children, BSONArrayBuilder& b)
{
  for (unsigned int ix = 0; ix < children.size(); ++ix)
  {
    orion::CompoundValueNode* child = children[ix];

    if (child->valueType == ValueTypeString)
    {
      b.append(child->stringValue);
    }
    else if (child->valueType == ValueTypeNumber)
    {
      b.append(child->numberValue);
    }
    else if (child->valueType == ValueTypeBoolean)
    {
      b.append(child->boolValue);
    }
    else if (child->valueType == ValueTypeNone)
    {
      b.appendNull();
    }
    else if (child->valueType == ValueTypeVector)
    {
      BSONArrayBuilder ba;

      compoundValueBson(child->childV, ba);
      b.append(ba.arr());
    }
    else if (child->valueType == ValueTypeObject)
    {
      BSONObjBuilder bo;

      compoundValueBson(child->childV, bo);
      b.append(bo.obj());
    }
    else
    {
      LM_E(("Runtime Error (Unknown type in compound value)"));
    }
  }
}



/* ****************************************************************************
*
* compoundValueBson (for objects) -
*/
void compoundValueBson(std::vector<orion::CompoundValueNode*> children, BSONObjBuilder& b)
{
  for (unsigned int ix = 0; ix < children.size(); ++ix)
  {
    orion::CompoundValueNode* child = children[ix];

    std::string effectiveName = dbDotEncode(child->name);

    if (child->valueType == ValueTypeString)
    {
      b.append(effectiveName, child->stringValue);
    }
    else if (child->valueType == ValueTypeNumber)
    {
      b.append(effectiveName, child->numberValue);
    }
    else if (child->valueType == ValueTypeBoolean)
    {
      b.append(effectiveName, child->boolValue);
    }
    else if (child->valueType == ValueTypeNone)
    {
      b.appendNull(effectiveName);
    }
    else if (child->valueType == ValueTypeVector)
    {
      BSONArrayBuilder ba;

      compoundValueBson(child->childV, ba);
      b.append(effectiveName, ba.arr());
    }
    else if (child->valueType == ValueTypeObject)
    {
      BSONObjBuilder bo;

      compoundValueBson(child->childV, bo);
      b.append(effectiveName, bo.obj());
    }
    else
    {
      LM_E(("Runtime Error (Unknown type in compound value)"));
    }
  }
}
