////////////////////////////////////////////////////////////////////////
// oipparms.cpp -- function parameter OIP calls:
//
//    AFuncGetParameter
//    AFuncGetTypedParameter
//    AFuncSetParameter
//
// jon 10-Jun-92 created
////////////////////////////////////////////////////////////////////////

	#include "almsys.h"

  #include "system.h"
  #include "context.h"

extern "C" {

//----------------------------------------------------------------------
//   AFuncGetParameter
//
// Retrieves an input parameter of currently executing function.
//----------------------------------------------------------------------
OBJECTID ALMAPI	AFuncGetParameter(
  LONG  parameterIndex)
  {
  return pTheSystem->GetCurrentContext()
           ->GetCurrFunc()
           ->GetParameter(parameterIndex);
  }


//----------------------------------------------------------------------
//   AFuncGetTypedParameter
//
// Retrieves an input parameter of currently executing function and
//
//   (1) verifies that it is of the specified type, or,
//   (2) coerces it to the correct type, or,
//   (3) returns zero.
//----------------------------------------------------------------------
OBJECTID ALMAPI	AFuncGetTypedParameter(
  LONG    parameterIndex,
  TYPEID  theType)
  {
  return pTheSystem->GetCurrentContext()
           -> GetCurrFunc()
           -> GetParameter(parameterIndex, theType);
  }

//----------------------------------------------------------------------
//   AFuncGetOutput
//
// Retrieves an output parameter of currently executing function.
//----------------------------------------------------------------------
OBJECTID ALMAPI	AFuncGetOutput(
  LONG      parameterIndex,
  LPTYPEID  theType)
  {
  return pTheSystem->GetCurrentContext()
           ->GetCurrFunc()
           ->GetOutputParameter(parameterIndex, theType);
  }

//----------------------------------------------------------------------
//   AFuncSetParameter
//
// Sets an output parameter of currently executing function.
//----------------------------------------------------------------------
BOOL ALMAPI	AFuncSetParameter(
  LONG      parameterIndex,
  OBJECTID  theObject)
  {
  return pTheSystem->GetCurrentContext()
           ->GetCurrFunc()
           ->SetParameter(parameterIndex, theObject);
  }

} // extern "C"
