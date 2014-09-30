//======================================================================
// oipexe.h -- calls into OIP.DLL from a kernel
//
//  jon 14-Jun-92 created
//  jon  9-Aug-92 added <Run,Continue>App
//  jon 22-Feb-93 exeRegisterContext now returns BOOL (was LONG).
//======================================================================

#include "classdef.h"

extern "C" {

  BOOL ALMAPI exeRegisterContext (LPEXEINFO theContext);
    //------------------------------------------------------------------
    // Registers the serius context for this task.  Unregisters any
    // previously registered context.  Returns FALSE on error.
    //------------------------------------------------------------------

  BOOL ALMAPI exeUnregisterContext();
    //------------------------------------------------------------------
    // Unregisters the serius context for this task.  Returns FALSE if
    // there is no serius context registered for this task.
    //------------------------------------------------------------------

  void ALMAPI exeRunApp();
    //------------------------------------------------------------------
    // Runs an application for this task.  Returns non-zero if we are
    // at a Stop.
    //------------------------------------------------------------------

  void ALMAPI exeContinueApp(BOOL);
    //------------------------------------------------------------------
    // Continues an application from a Stop.  Returns non-zero if we
    // hit another Stop.
    //------------------------------------------------------------------

  void ALMAPI exeResetApp();
    //------------------------------------------------------------------
    // Kills the application from a Stop point.
    //------------------------------------------------------------------

} // extern "C"
